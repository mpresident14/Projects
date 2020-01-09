#include "parser.hpp"
#include "widget.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <optional>


using namespace std;
using namespace parsers;

class Expr {
public:
    virtual ~Expr() = 0;
    virtual void print() = 0;
    virtual int eval() = 0;
};

Expr::~Expr(){}

class Term : public Expr {
public:
    Term(int n) : num_{n} {}
    void print() override { cout << num_ << endl; }
    int eval() override { return num_; }
    int num_;
};

enum Op {PLUS, MINUS, TIMES, DIVIDE};

char opToSym[]{'+', '-', 'x', '/'};

class BinOp : public Expr {
public:
    BinOp(Expr *e1, Expr *e2,  Op op) : e1_{e1}, e2_{e2}, op_{op} {}
    ~BinOp() { delete e1_; delete e2_; }
    void print() override { e1_->print(); cout << opToSym[op_] << endl; e2_->print(); }
    int eval() override
    {
        int n = e1_->eval();
        int m = e2_->eval();
        switch(op_) {
            case PLUS:
                return n + m;
            case MINUS:
                return n - m;
            case TIMES:
                return n * m;
            case DIVIDE:
                return n / m;
        }
    }
    Expr *e1_;
    Expr *e2_;
    Op op_;
};

int main(int argc, char **argv)
{
    Parser<optional<pair<Op, Expr*>>> exprRest = fail<optional<pair<Op, Expr*>>>;

    // THIS IS BACKWARDS
    Parser<Expr*> expr =
        anyDigit.combineRef(exprRest).andThenMap(
            [](pair<int, optional<pair<Op, Expr*>>> termExprRest) {
                optional<pair<Op, Expr*>> rest = get<1>(termExprRest);

                // cout << "*****************" << endl;
                // cout << "size: " << exprRest.size() << endl;
                // for (auto& ppair : exprRest) {
                //     cout << opToSym[ppair.first] << endl;
                //     get<1>(ppair)->print();
                // }
                // cout << "*****************" << endl;

                if (!rest.has_value()) {
                    return (Expr*) new Term(termExprRest.first);
                } else {
                    return (Expr*) new BinOp(new Term(termExprRest.first), get<1>(rest.value()), rest.value().first);
                }
            }
        );

    Parser<Op> plusMinus = 
        thisChar('+').andThenMap([](char) { return PLUS; })
        .alt(thisChar('-').andThenMap([](char) { return MINUS; }));
    
    exprRest = 
        plusMinus
        .combine(expr)
        .andThenMap([](auto ppair) { return make_optional(ppair); })
        .alt(createBasic( (optional<pair<Op, Expr*>>) nullopt));

    try {
        auto result = expr.parse(argv[1]);
        result->print();
        cout << result->eval() << endl;
        delete result;
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
}