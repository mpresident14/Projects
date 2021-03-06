#include "expr.hpp"

using namespace std;
using namespace parsers;
using ExprPtr = unique_ptr<Expr>;

ExprPtr foldExprs(pair<ExprPtr, vector<pair<Op, ExprPtr>>> exprAndRestVec) {
  auto& restVec = get<1>(exprAndRestVec);
  ExprPtr binop = move(exprAndRestVec.first);

  for (auto& opAndExpr : restVec) {
    binop = make_unique<BinOp>(move(binop), move(opAndExpr.second), opAndExpr.first);
  }
  return binop;
}

void doParse(const char* input) {
  Parser<ExprPtr> expr = fail<ExprPtr>();  // Placeholder

  Parser<Op> plusMinus =
      skipws(thisChar('+')
                 .transform([](char) { return PLUS; })
                 .alt(thisChar('-').transform([](char) { return MINUS; })));
  Parser<Op> timesDiv =
      skipws(thisChar('*')
                 .transform([](char) { return TIMES; })
                 .alt(thisChar('/').transform([](char) { return DIVIDE; })));
  Parser<Op> power = skipws(thisChar('^').transform([](char) { return POW; }));

  Parser<ExprPtr> single = skipws(
      anyUDouble.transform([](double n) -> ExprPtr { return make_unique<Num>(n); })
          .alt(thisChar('(').ignoreAndThen(expr).thenIgnore(skipws(thisChar(')')))));
  Parser<vector<pair<Op, ExprPtr>>> restPowers = power.combine(single).many();

  Parser<ExprPtr> factor = fail<ExprPtr>();
  factor.set(
      single.combine(restPowers)
          .transform(foldExprs)
          .alt(thisChar('-').ignoreAndThen(factor).transform([](ExprPtr exp) -> ExprPtr {
            return make_unique<BinOp>(make_unique<Num>(-1), move(exp), TIMES);
          })));

  // TODO: This should fail but doesn't. Shouldn't be able to copy "factor" after being
  // set.
  Parser<vector<pair<Op, ExprPtr>>> restFactors = timesDiv.combine(factor).many();

  Parser<ExprPtr> term = factor.combine(restFactors).transform(foldExprs);
  Parser<vector<pair<Op, ExprPtr>>> restTerms = plusMinus.combine(term).many();

  expr.set(term.combine(restTerms).thenIgnore(whitespace).transform(foldExprs));

  try {
    ExprPtr result = expr.parse(input);
    cout << result->eval() << endl;
  } catch (invalid_argument& e) {
    cerr << e.what() << endl;
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "Enter a string to parse." << endl;
    return 1;
  }

  // NOTE: We can't do "Expr := Term | Expr Op Expr" b/c this would cause
  // infinite recursion when trying to parse the right choice.

  /*
   * Expr         := Term RestTerms
   * RestTerms    := [+|- Term]
   * Term         := Factor RestFactors
   * RestFactors  := [*|/ Factor]
   * Factor       := Single RestPowers | -Factor
   * RestPowers   := [^ Single]
   * Single       := uDouble | (Expr)
   */

  doParse(argv[1]);
  return 0;
}
