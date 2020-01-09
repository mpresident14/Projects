#include "parser.hpp"
#include "widget.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <memory>

using namespace std;
using namespace parsers;

class Expr {
public:
    virtual ~Expr(){};
    virtual double eval() = 0;
};

using ExprPtr = unique_ptr<Expr>;

class Num : public Expr {
public:
    Num(double n) : num_{n} {}
    double eval() override { return num_; }

private:
    double num_;
};

enum Op {PLUS, MINUS, TIMES, DIVIDE, POW};

class BinOp : public Expr {
public:
    BinOp(ExprPtr e1, ExprPtr e2,  Op op) : e1_{move(e1)}, e2_{move(e2)}, op_{op} {}
    double eval() override
    {
        double n = e1_->eval();
        double m = e2_->eval();
        switch(op_) {
            case PLUS:
                return n + m;
            case MINUS:
                return n - m;
            case TIMES:
                return n * m;
            case DIVIDE:
                return n / m;
            case POW:
                return pow(n, m);
        }
    }

private:
    ExprPtr e1_;
    ExprPtr e2_;
    Op op_;
};

ExprPtr foldExprs(pair<ExprPtr, vector<pair<Op, ExprPtr>>> exprAndRestVec)
{
    auto& restVec = get<1>(exprAndRestVec);
    ExprPtr binop = move(exprAndRestVec.first);

    for (auto& opAndExpr : restVec) {
        binop = make_unique<BinOp>(move(binop), move(opAndExpr.second), opAndExpr.first);
    }
    return binop;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        cerr << "Enter a string to parse." << endl;
        return 1;
    }
    
    // NOTE: We can't do "Expr := Term | Expr Op Expr" b/c this would cause infinite recursion
    // when trying to parse the right choice.

    /* 
     * Expr         := Term RestTerms
     * RestTerms    := [+|- Term]
     * Term         := Factor RestFactors
     * RestFactors  := [*|/ Factor]
     * Factor       := Single RestPowers | -Factor
     * RestPowers   := [^ Single]
     * Single       := uDouble | (Expr)
     */

    Parser<ExprPtr> expr =  fail<ExprPtr>; // Placeholder

    Parser<Op> plusMinus = 
        skipws(
            thisChar('+').andThenMap([](char) { return PLUS; })
            .alt(thisChar('-').andThenMap([](char) { return MINUS; }))
        );
    Parser<Op> timesDiv = 
        skipws(
            thisChar('*').andThenMap([](char) { return TIMES; })
            .alt(thisChar('/').andThenMap([](char) { return DIVIDE; }))
        );
    Parser<Op> power = skipws(thisChar('^').andThenMap([](char) { return POW; }));

    Parser<ExprPtr> single =
        skipws(
            anyUDouble.andThenMap([](double n) -> ExprPtr  { return make_unique<Num>(n); })
            .alt(
                thisChar('(')
                .ignoreAndThenRef(expr) // Needs to be a reference b/c expr is not yet defined
                .thenIgnore(skipws(thisChar(')'))))
        );
    Parser<vector<pair<Op, ExprPtr>>> restPowers = power.combine(single).many();

    Parser<ExprPtr> factor =
        single.combine(restPowers).andThenMap(foldExprs)
        .alt(thisChar('-').ignoreAndThenRef(factor).andThenMap(
            [](ExprPtr expr) -> ExprPtr {
                return make_unique<BinOp>(make_unique<Num>(-1), move(expr), TIMES); 
            }
        ));

    Parser<vector<pair<Op, ExprPtr>>> restFactors = timesDiv.combine(factor).many();

    Parser<ExprPtr> term = factor.combine(restFactors).andThenMap(foldExprs);
    Parser<vector<pair<Op, ExprPtr>>> restTerms = plusMinus.combine(term).many();

    expr = term.combine(restTerms).thenIgnore(whitespace).andThenMap(foldExprs);
    
    try {
        ExprPtr result = expr.parse(argv[1]);
        cout << result->eval() << endl;
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }

    return 0;
}