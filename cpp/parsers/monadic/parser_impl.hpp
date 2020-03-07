// Only need to specify default template arg (Void) once (in header file)
template <typename T>
template <typename Fn, typename Void>
Parser<T>::Parser(Fn&& f)
    : parseFn_{std::make_shared<FnContainer<Fn>>(std::forward<Fn>(f))} {}

template <typename T>
T Parser<T>::parse(const std::string& input) const {
  using namespace std;

  size_t errPos = 0;
  result_t<T> optResult = (*parseFn_)(input, &errPos);
  if (!optResult.has_value()) {
    throw invalid_argument("Parse error: failed at: \"" + input.substr(errPos) + "\"");
  }

  pair<T, string_view>& pairResult = optResult.value();
  string_view& rest = get<1>(pairResult);
  if (!rest.empty()) {
    throw invalid_argument("No parse: \"" + string(rest) + "\" remained.");
  }

  // Need "move" since the parsed object is inside the pair.
  // Prevents a call to the copy constructor.
  return move(get<0>(pairResult));
}

template <typename T>
template <typename Fn, typename Par>
std::enable_if_t<is_parser_v<Par>, Par> Parser<T>::andThen(Fn&& pGenFn) const {
  using namespace std;

  return {// This must be mutable to call pGenFn's non-const () operator
          [parseFn = isThisRValue() ? move(parseFn_) : parseFn_,
           pGenFn = forward<Fn>(pGenFn)](
              input_t& input, size_t *errPos) mutable -> result_t<ptype_t<Par>> {
            // Run first parser
            result_t<T> optResult = (*parseFn)(input, errPos);
            // If first parser fails, fail entire thing
            if (!optResult.has_value()) {
              return {};
            }

            pair<T, string_view>& pairResult = optResult.value();
            T& result = get<0>(pairResult);
            auto nextParser = pGenFn(move(result));
            // Run the next parser on the rest of the string
            return (*nextParser.parseFn_)(get<1>(pairResult), errPos);
          }};
}

template <typename T>
template <typename Fn, typename Par>
std::enable_if_t<is_parser_v<Par>, Par> Parser<T>::andThenRef(
    Fn&& pGenFn) const {
  using namespace std;

  return {// This must be mutable to call pGenFn's non-const () operator
          [this, pGenFn = forward<Fn>(pGenFn)](
              input_t& input, size_t *errPos) mutable -> result_t<ptype_t<Par>> {
            // Run first parser
            result_t<T> optResult = (*parseFn_)(input, errPos);
            // If first parser fails, fail entire thing
            if (!optResult.has_value()) {
              return {};
            }

            pair<T, string_view>& pairResult = optResult.value();
            T& result = get<0>(pairResult);
            auto nextParser = pGenFn(move(result));
            // Run the next parser on the rest of the string
            return (*nextParser.parseFn_)(get<1>(pairResult), errPos);
          }};
}

template <typename T>
Parser<T> Parser<T>::alt(Parser<T> nextParser) const {
  using namespace std;

  return {[parseFn = isThisRValue() ? move(parseFn_) : parseFn_,
           nextParser = move(nextParser)](input_t& input, size_t *errPos) {
    // Run first parser
    result_t<T> optResult = (*parseFn)(input, errPos);
    // If first parser succeeds, return the result
    if (optResult.has_value()) {
      return optResult;
    }

    // Otherwise, try the next parser on the input
    return (*nextParser.parseFn_)(input, errPos);
  }};
}

template <typename T>
Parser<T> Parser<T>::altRef(const Parser<T>& nextParser) const {
  using namespace std;

  return {[parseFn = isThisRValue() ? move(parseFn_) : parseFn_,
           &nextParser](input_t& input, size_t *errPos) {
    // Run first parser
    result_t<T> optResult = (*parseFn)(input, errPos);
    // If first parser succeeds, return the result
    if (optResult.has_value()) {
      return optResult;
    }

    // Otherwise, try the next parser on the input
    return (*nextParser.parseFn_)(input, errPos);
  }};
}

template <typename T>
template <typename Fn, typename R>
Parser<std::decay_t<R>> Parser<T>::andThenMap(Fn&& mapFn) const {
  using namespace std;

  return andThen([mapFn = forward<Fn>(mapFn)](T&& obj) mutable {
    return parsers::createBasic(mapFn(forward<T>(obj)));
  });
}

// TODO: expand this to use a tuple instead of a pair (for longer chains) (use
// varargs)

// Pass nextParser by value since we have to copy it into the lambda anyways.
template <typename T>
template <typename R>
Parser<std::pair<T, R>> Parser<T>::combine(Parser<R> nextParser) const {
  using namespace std;

  return andThen([nextParser = move(nextParser)](T&& obj1) {
    return nextParser.andThen(
        // In order to forward obj1, the lambda must be mutable
        [obj1 = forward<T>(obj1)](R&& obj2) mutable {
          return parsers::createBasic(
              make_pair(forward<T>(obj1), forward<R>(obj2)));
        });
  });
}

template <typename T>
template <typename R>
Parser<std::pair<T, R>> Parser<T>::combineRef(
    const Parser<R>& nextParser) const {
  using namespace std;

  return andThen([&nextParser](T&& obj1) {
    return nextParser.andThenRef(
        // In order to forward obj1, the lambda must be mutable
        [obj1 = forward<T>(obj1)](R&& obj2) mutable {
          return parsers::createBasic(
              make_pair(forward<T>(obj1), forward<R>(obj2)));
        });
  });
}

template <typename T>
template <typename Fn, typename Void>
Parser<T> Parser<T>::verify(Fn&& boolFn) const {
  using namespace std;

  return andThen([boolFn = forward<Fn>(boolFn)](T&& obj) mutable -> Parser<T> {
    if (!boolFn(forward<T>(obj))) {
      return parsers::fail<T>;
    }
    return parsers::createBasic(forward<T>(obj));
  });
}

template <typename T>
template <typename U>
Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> Parser<T>::many()
    const {
  using namespace std;

  return Parser<string>{
      [parseFn = isThisRValue() ? move(parseFn_) : parseFn_](input_t& input, size_t *errPos) {
        // Run parser until it fails and put each result in the list
        unsigned i = 0;
        while ((*parseFn)(input.substr(i), errPos).has_value()) {
          ++i;
        }
        // TODO: RESET errPos HERE BECAUSE MANY WILL NEVER FAIL

        return parsers::createReturnObject(
            string(input.substr(0, i)), input.substr(i));
      }};
}

template <typename T>
template <typename U>
Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>>
Parser<T>::many() const {
  using namespace std;

  return Parser<vector<T>>{
      [parseFn = isThisRValue() ? move(parseFn_) : parseFn_](input_t& input, size_t *errPos) {
        // Run parser until it fails and put each result in the list
        vector<T> listResult;
        unsigned i = 0;
        result_t<T> optResult = (*parseFn)(input, errPos);
        while (optResult.has_value()) {
          pair<T, string_view>& pairResult = optResult.value();
          listResult.push_back(move(get<0>(pairResult)));
          i = input.size() - get<1>(pairResult).size();
          optResult = (*parseFn)(get<1>(pairResult), errPos);
        }

        return parsers::createReturnObject(move(listResult), input.substr(i));
      }};
}

template <typename T>
template <typename U>
Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> Parser<T>::some()
    const {
  using namespace std;

  return combine(many()).andThenMap([](auto&& charAndString) {
    auto& str = get<1>(charAndString);
    str.insert(str.cbegin(), move(get<0>(charAndString)));
    return move(str);
  });
}

template <typename T>
template <typename U>
Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>>
Parser<T>::some() const {
  using namespace std;

  return combine(many()).andThenMap([](auto&& objAndObjVec) {
    auto& objVec = get<1>(objAndObjVec);
    objVec.insert(objVec.begin(), move(get<0>(objAndObjVec)));
    return move(objVec);
  });
}

template <typename T>
Parser<std::nullptr_t> Parser<T>::ignore() const {
  return andThenMap([](T&&) { return nullptr; });
}

template <typename T>
template <typename R>
Parser<R> Parser<T>::ignoreAndThen(Parser<R> nextParser) const {
  using namespace std;

  return andThen(
      [nextParser = move(nextParser)](T&&) { return move(nextParser); });
}

template <typename T>
template <typename R>
Parser<T> Parser<T>::thenIgnore(Parser<R> nextParser) const {
  using namespace std;

  return andThen([nextParser = move(nextParser)](T&& obj1) {
    return nextParser.andThenMap(
        // In order to forward obj1, the lambda must be mutable
        [obj1 = forward<T>(obj1)](R&&) mutable { return forward<T>(obj1); });
  });
}

template <typename T>
template <typename R>
Parser<R> Parser<T>::ignoreAndThenRef(const Parser<R>& nextParser) const {
  using namespace std;

  return andThen([&nextParser](T&&) { return nextParser; });
}

template <typename T>
template <typename R>
Parser<T> Parser<T>::thenIgnoreRef(const Parser<R>& nextParser) const {
  using namespace std;

  return andThen([&nextParser](T&& obj1) {
    return nextParser.andThenRef(
        // In order to forward obj1, the lambda must be mutable
        [obj1 = forward<T>(obj1)](R&&) mutable {
          return parsers::createBasic(forward<T>(obj1));
        });
  });
}
