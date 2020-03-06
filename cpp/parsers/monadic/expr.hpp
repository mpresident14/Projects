#include "parser.hpp"

#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Expr {
public:
  virtual ~Expr(){};
  virtual double eval() = 0;
};

using ExprPtr = std::unique_ptr<Expr>;

class Num : public Expr {
public:
  Num(double n) : num_{n} {}
  double eval() override { return num_; }

private:
  double num_;
};

enum Op { PLUS, MINUS, TIMES, DIVIDE, POW };

class BinOp : public Expr {
public:
  BinOp(ExprPtr e1, ExprPtr e2, Op op)
      : e1_{std::move(e1)}, e2_{std::move(e2)}, op_{op} {}
  double eval() override {
    double n = e1_->eval();
    double m = e2_->eval();
    switch (op_) {
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
