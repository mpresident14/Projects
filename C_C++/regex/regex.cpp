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

Parser<unique_ptr<Regex>> Regex::kConcatParser = 
    parsers::success
    .ignoreAndThenRef(Regex::kRegexParser)
    .combineRef(Regex::kRegexParser)
    .andThenMap(
        [](auto rgxPair) {
            return (unique_ptr<Regex>) make_unique<Concat>(Concat(move(rgxPair.first), move(rgxPair.second)));
        }
    );


/* TODO: HOW TO CHANGE INTERFACE TO ALLOW RECURSIVE DEFINITION OF A PARSER? */
Parser<unique_ptr<Regex>> Regex::kRegexParser = 
    Regex::kDotParser
    /* OK IF WE DEFINE kCharParser BEFORE (not possible if we replaced it with kRegexParser 
     * to define recursively) */
    // .alt(Regex::kCharParser)
    /* DANGLING REFERENCE */
    // .altRef(thisChar('j').andThenMap([](char c) { return (unique_ptr<Regex>) make_unique<Character>(Character(c)); } ));
    .alt(Regex::kCharParser)
    .alt(Regex::kConcatParser);




/*****************************
 *          DOT              *
 *****************************/
void Dot::print() const
{
    cout << "DOT" << endl;
}


/*****************************
 *        CHARACTER          *
 *****************************/
Character::Character(char c) : c_{c} {}

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

void Concat::print() const
{
    cout << "CONCAT" << endl;
    rgx1_->print();
    rgx2_->print();
    cout << endl;
}