// Only need to specify default template arg (Void) once (in header file)
template <typename T>
template <typename Fn, typename Void>
Parser<T>::Parser(Fn&& f)
    : parseFn_(std::make_shared<std::unique_ptr<FnContainerAbstract>>(std::make_unique<FnContainer<Fn>>(std::forward<Fn>(f)))) {}

// : parseFn_(std::make_shared<std::function<result_t<T>(input_t&, size_t*)>>(
//       std::forward<Fn>(f))) {}

template <typename T>
T Parser<T>::parse(const std::string& input) const {
  using namespace std;

  size_t errPos = 0;
  stringstream inputStream(input);
  result_t<T> optResult = (**parseFn_)(inputStream, &errPos);
  if (!optResult.has_value()) {
    string line;
    inputStream.seekg(errPos);
    getline(inputStream, line);
    throw invalid_argument("Parse error: Failed at: \"" + line + "\"");
  }

  if (inputStream.peek() != EOF) {
    string line;
    getline(inputStream, line);
    throw invalid_argument("Parser error: Characters remain at: \"" + line + "\"");
  }

  return move(optResult.value());
}

/* andThen makes the guarantee that the parser in creates will reset the input stream
 * position if either parser fails. Combinators using andThen do not have to reset the
 * input stream */
template <typename T>
template <typename Fn, typename Par>
std::enable_if_t<is_parser_v<Par>, Par> Parser<T>::andThen(Fn&& pGenFn) const {
  using namespace std;

  return {// This must be mutable to call pGenFn's non-const () operator
      [parseFn = isThisRValue() ? move(parseFn_) : parseFn_,
          pGenFn = forward<Fn>(pGenFn)](
          input_t& input, size_t* errPos) mutable -> result_t<ptype_t<Par>> {
        // Always reset to original position if either parser fails
        size_t origPos = input.tellg();

        // Run first parser
        result_t<T> optResult1 = (**parseFn)(input, errPos);
        // If first parser fails, reset the stream and fail entire thing
        if (!optResult1.has_value()) {
          input.seekg(origPos);
          *errPos = max(*errPos, origPos);
          return {};
        }

        // Run the next parser on the rest of the string
        auto nextParser = pGenFn(move(optResult1.value()));
        auto optResult2 = (**nextParser.parseFn_)(input, errPos);
        if (optResult2.has_value()) {
          return optResult2;
        }

        *errPos = max(*errPos, origPos);
        input.seekg(origPos);
        return optResult2;
      }};
}

template <typename T>
template <typename Fn, typename Par>
std::enable_if_t<is_parser_v<Par>, Par> Parser<T>::andThenRef(Fn&& pGenFn) const {
  using namespace std;

  return {// This must be mutable to call pGenFn's non-const () operator
      [this, pGenFn = forward<Fn>(pGenFn)](
          input_t& input, size_t* errPos) mutable -> result_t<ptype_t<Par>> {
        // Always reset to original position if either parser fails
        size_t origPos = input.tellg();

        // Run first parser
        result_t<T> optResult1 = (**parseFn_)(input, errPos);
        // If first parser fails, reset the stream and fail entire thing
        if (!optResult1.has_value()) {
          input.seekg(origPos);
          *errPos = max(*errPos, origPos);
          return {};
        }

        // Specify the error location at which the deepest parsing attempt failed
        // Run the next parser on the rest of the string
        auto nextParser = pGenFn(move(optResult1.value()));
        auto optResult2 = (**nextParser.parseFn_)(input, errPos);
        if (optResult2.has_value()) {
          return optResult2;
        }

        *errPos = max(*errPos, origPos);
        input.seekg(origPos);
        return optResult2;
      }};
}

template <typename T>
Parser<T> Parser<T>::alt(Parser<T> nextParser) const {
  using namespace std;

  return {[parseFn = isThisRValue() ? move(parseFn_) : parseFn_,
              nextParser = move(nextParser)](input_t& input, size_t* errPos) {
    // Run first parser
    result_t<T> optResult1 = (**parseFn)(input, errPos);
    // If first parser succeeds, return the result
    if (optResult1.has_value()) {
      return optResult1;
    }

    // Otherwise, try the next parser on the input
    result_t<T> optResult2 = (**nextParser.parseFn_)(input, errPos);
    if (optResult2.has_value()) {
      return optResult2;
    }
    return optResult2;
  }};
}

template <typename T>
Parser<T> Parser<T>::altRef(const Parser<T>& nextParser) const {
  using namespace std;

  return {[parseFn = isThisRValue() ? move(parseFn_) : parseFn_, &nextParser](
              input_t& input, size_t* errPos) {
    // Run first parser
    result_t<T> optResult1 = (**parseFn)(input, errPos);
    // If first parser succeeds, return the result
    if (optResult1.has_value()) {
      return optResult1;
    }

    size_t firstErrPos = *errPos;
    // Otherwise, try the next parser on the input
    result_t<T> optResult2 = (**nextParser.parseFn_)(input, errPos);
    if (optResult2.has_value()) {
      return optResult2;
    }
    // Set the error that was farther along
    *errPos = max(*errPos, firstErrPos);
    return optResult2;
  }};
}

template <typename T>
template <typename Fn, typename R>
Parser<std::decay_t<R>> Parser<T>::andThenMap(Fn&& mapFn) const {
  using namespace std;

  // Safe to move obj because andThen() will not need it again.
  return andThen([mapFn = forward<Fn>(mapFn)](
                     T&& obj) mutable { return parsers::createBasic(mapFn(move(obj))); });
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
        // In order to move obj1, the lambda must be mutable
        // Safe to move obj1 and obj2 because of andThen() does not need their
        // values anymore.
        [obj1 = move(obj1)](R&& obj2) mutable {
          return parsers::createBasic(make_pair(move(obj1), move(obj2)));
        });
  });
}

template <typename T>
template <typename R>
Parser<std::pair<T, R>> Parser<T>::combineRef(const Parser<R>& nextParser) const {
  using namespace std;

  return andThen([&nextParser](T&& obj1) {
    return nextParser.andThenRef(
        // In order to forward obj1, the lambda must be mutable
        [obj1 = forward<T>(obj1)](R&& obj2) mutable {
          return parsers::createBasic(make_pair(forward<T>(obj1), forward<R>(obj2)));
        });
  });
}

template <typename T>
template <typename Fn, typename Void>
Parser<T> Parser<T>::verify(Fn&& boolFn) const {
  using namespace std;

  return andThen([boolFn = forward<Fn>(boolFn)](T&& obj) mutable -> Parser<T> {
    // Safe to move obj because andThen() will not need it again.
    if (!boolFn(move(obj))) {
      return parsers::fail<T>;
    }
    return parsers::createBasic(move(obj));
  });
}

template <typename T>
template <typename U>
Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> Parser<T>::many() const {
  using namespace std;

  return Parser<string>{[parseFn = isThisRValue() ? move(parseFn_) : parseFn_](
                            input_t& input, size_t* errPos) {
    size_t oldErrPos = *errPos;
    // Run parser until it fails and put each result in the list
    string parsedChars;
    result_t<char> optResult = (**parseFn)(input, errPos);
    while (optResult.has_value()) {
      // value() is a char, no need to move it
      parsedChars.append(1, optResult.value());
      optResult = (**parseFn)(input, errPos);
    }

    // Reset errPos because many() does not fail when the underlying parser fails.
    *errPos = oldErrPos;
    return parsers::createReturnObject(move(parsedChars));
  }};
}

template <typename T>
template <typename U>
Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>> Parser<T>::many()
    const {
  using namespace std;

  return Parser<vector<T>>{[parseFn = isThisRValue() ? move(parseFn_) : parseFn_](
                               input_t& input, size_t* errPos) {
    size_t oldErrPos = *errPos;
    // Run parser until it fails and put each result in the list
    vector<T> parsedObjs;
    result_t<T> optResult = (**parseFn)(input, errPos);
    while (optResult.has_value()) {
      parsedObjs.push_back(move(optResult.value()));
      optResult = (**parseFn)(input, errPos);
    }

    // Reset errPos because many() does not fail when the underlying parser fails
    *errPos = oldErrPos;
    return parsers::createReturnObject(move(parsedObjs));
  }};
}

// TODO: Could use a deque here (and thus for many() as well)
template <typename T>
template <typename U>
Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> Parser<T>::some() const {
  using namespace std;

  return combine(many()).andThenMap([](auto&& charAndString) {
    auto& str = get<1>(charAndString);
    str.insert(str.cbegin(), move(get<0>(charAndString)));
    return move(str);
  });
}

template <typename T>
template <typename U>
Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>> Parser<T>::some()
    const {
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

  return andThen([nextParser = move(nextParser)](T&&) { return move(nextParser); });
}

template <typename T>
template <typename R>
Parser<T> Parser<T>::thenIgnore(Parser<R> nextParser) const {
  using namespace std;

  return andThen([nextParser = move(nextParser)](T&& obj) {
    return nextParser.andThenMap(
        // In order to forward obj, the lambda must be mutable
        // Safe to move obj because andThen() will not need it again.
        [obj = move(obj)](R&&) mutable { return move(obj); });
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
        [obj1 = forward<T>(obj1)](
            R&&) mutable { return parsers::createBasic(forward<T>(obj1)); });
  });
}

template <typename T>
template <typename R>
void Parser<T>::set(Parser<R>&& other) {
  *parseFn_ = std::move(*other.parseFn_);
}
