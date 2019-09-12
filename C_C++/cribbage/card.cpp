#include "card.hpp"

using namespace std;

Card::Card(size_t value, Suit suit)
    : value_{value}, suit_{suit}, adder_{value} {
  if (adder_ > 10) {
    adder_ = 10;
  }
}

size_t Card::value() const { return value_; }

Suit Card::suit() const { return suit_; }

size_t Card::adder() const { return adder_; }

std::ostream& Card::print(std::ostream& out) const {
  switch (value_) {
    case 1:
      out << "A"
          << " " << suit_;
      break;
    case 11:
      out << "J"
          << " " << suit_;
      break;
    case 12:
      out << "Q"
          << " " << suit_;
      break;
    case 13:
      out << "K"
          << " " << suit_;
      break;
    default:
      out << value_ << " " << suit_;
      break;
  }
  return out;
}

size_t Card::operator+(const size_t& num) const { return value_ + num; }

bool Card::operator==(const size_t& num) const { return value_ == num; }

size_t Card::operator+(const Card& c) const { return value_ + c.value_; }

bool Card::operator==(const Card& c) const { return value_ == c.value_; }

bool Card::operator<(const Card& c) const { return value_ < c.value_; }

bool Card::equals(const Card& c) const {
  return value_ == c.value_ && suit_ == c.suit_;
}

// int main()
// {
//     Card c(2, HEARTS);
//     Card d(2, DIAMONDS);
//     Card e(3, DIAMONDS);
//     Card f(4, DIAMONDS);
//     Card g(5, DIAMONDS);
//     Card* hand = new Card[5] {c,d,e,f,g};
//     size_t n = hand[0] + hand[2] + hand[4];
//     cout << n << endl;
// }