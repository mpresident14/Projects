#include "parser.hpp"
#include "widget.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <vector>
#include <cmath>

using namespace std;
using namespace parsers;

class Expr {
public:
    virtual ~Expr(){};
    virtual double eval() = 0;
};

class Num : public Expr {
public:
    Num(double n) : num_{n} {}
    double eval() override { return num_; }
    double num_;
};

enum Op {PLUS, MINUS, TIMES, DIVIDE, POW};
char opToSym[]{'+', '-', 'x', '/'};

class BinOp : public Expr {
public:
    BinOp(Expr *e1, Expr *e2,  Op op) : e1_{e1}, e2_{e2}, op_{op} {}
    ~BinOp() { delete e1_; delete e2_; }
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
    Expr *e1_;
    Expr *e2_;
    Op op_;
};

Expr* foldExprs(pair<Expr*, vector<pair<Op, Expr*>>> exprAndRestVec)
{
    auto& restVec = get<1>(exprAndRestVec);
    if (restVec.empty()) {
        return exprAndRestVec.first;
    }

    BinOp *binop;
    Expr *eLeft = exprAndRestVec.first;
    for (auto& opAndExpr : restVec) {
        binop = new BinOp(eLeft, opAndExpr.second, opAndExpr.first);
        eLeft = binop;
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
     * Factor       := Single RestPowers
     * RestPowers   := [^ Single]
     * Single       := double | (Expr)
     */

    Parser<Expr*> expr =  fail<Expr*>; // Placeholder

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

    Parser<Expr*> single =
        skipws(
            anyDouble.andThenMap([](double n) -> Expr* { return new Num(n); })
            .alt(
                thisChar('(')
                .ignoreAndThenRef(expr) // Needs to be a reference b/c expr is not yet defined
                .thenIgnore(skipws(thisChar(')'))))
        );
    Parser<vector<pair<Op, Expr*>>> restPowers = power.combine(single).many();

    Parser<Expr*> factor = single.combine(restPowers).andThenMap(foldExprs);
    Parser<vector<pair<Op, Expr*>>> restFactors = timesDiv.combine(factor).many();

    Parser<Expr*> term = factor.combine(restFactors).andThenMap(foldExprs);
    Parser<vector<pair<Op, Expr*>>> restTerms = plusMinus.combine(term).many();

    expr = term.combine(restTerms).thenIgnore(whitespace).andThenMap(foldExprs);
    
    try {
        auto result = expr.parse(argv[1]);
        cout << result->eval() << endl;
        delete result;
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }

    return 0;
}