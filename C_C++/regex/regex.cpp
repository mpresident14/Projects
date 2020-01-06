#include "regex.hpp"

#include <iostream>

using namespace std;
using namespace parsers;

Parser<unique_ptr<Regex>> Regex::kDotParser = 
    thisChar('.')
    .andThenMap([](char) { return (unique_ptr<Regex>) make_unique<Dot>(Dot()); } );

Parser<unique_ptr<Regex>> Regex::kCharParser = 
    anyChar
    .andThenMap([](char c) { return (unique_ptr<Regex>) make_unique<Character>(Character(c)); } );


Parser<unique_ptr<Regex>> Regex::kRegexParser = 
    Regex::kDotParser
    .alt(Regex::kCharParser);

/*****************************
 *          DOT              *
 *****************************/
// Dot::~Dot(){}

void Dot::print() const
{
    cout << "DOT" << endl;
}


/*****************************
 *        CHARACTER          *
 *****************************/
Character::Character(char c) : c_{c} {}

// Character::~Character(){}

void Character::print() const
{
    cout << "CHAR " << c_ << endl;
}

/*****************************
 *          CONCAT           *
 *****************************/
Concat::Concat(unique_ptr<Regex> rgx1, unique_ptr<Regex>rgx2)
    : rgx1_{move(rgx1)}, rgx2_{move(rgx2)}
{}
