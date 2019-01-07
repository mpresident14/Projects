#ifndef CARD_HPP_INCLUDED
#define CARD_HPP_INCLUDED 1

#include <iostream>
#include <cstddef>
#include <string>
#include <vector>

enum Suit 
        {
            HEARTS,
            DIAMONDS,
            SPADES,
            CLUBS
        };

class Card{
    public:
        

        Card() = delete;
        Card(size_t value, Suit suit);
        std::ostream& print(std::ostream& out) const;
        std::ostream& printEnum(std::ostream& out, const Suit& s) const;
        size_t operator+ (const size_t& num) const; /// adds card value to integral type  
        bool operator== (const size_t& num) const; /// compares card value with integral type

        size_t operator+ (const Card& c) const; /// Adds cards' values
        bool operator== (const Card& c) const; /// based on card's VALUE ONLY
        bool operator< (const Card& c) const; ///based on card's VALUE ONLY
        bool equals(const Card& c) const; /// based on card's VALUE AND SUIT

        size_t value() const;
        Suit suit() const;
        size_t adder() const;

    private:
        size_t value_;
        Suit suit_;
        size_t adder_;
};

inline std::ostream& operator<< (std::ostream& out, const Card& c)
{
    return c.print(out);
}

inline size_t operator+ (const size_t& num, const Card& c){     
    return num + c.value();
} 

/// To print an Enum
inline std::ostream& operator<< (std::ostream& out, const Suit& s)
{
    switch(s){
        case 0:
            out << "Hearts";
            break;
        case 1:
            out << "Diamonds";
            break;
        case 2:
            out << "Spades";
            break;
        default:
            out << "Clubs";
            break;
    }
    return out;
}

inline size_t& operator+= (size_t& num, const Card& c)
{
    num = num + c;
    return num;
}

inline void printCardArray(Card**& cards, size_t length)
{
    std::cout << "[";

    for (size_t i = 0; i < length; ++i){
        std::cout << *(cards[i]);        
        if (i != length - 1){
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

inline void printCardVector(std::vector<Card*>& cards)
{
    std::cout << "[";

    for (size_t i = 0; i < cards.size(); ++i){
        std::cout << *(cards[i]);        
        if (i != cards.size() - 1){
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

inline Card** copyCardArray(Card**& cards, size_t length)
{
    Card** newArray = new Card*[length];
    for (size_t i = 0; i < length; ++i){
        newArray[i] = cards[i];
    }
    return newArray;
}

inline void destroyCardArray(Card**& cards, size_t length)
{
    for (size_t i = 0; i < length; ++i){
        delete cards[i];
    }
    delete[] cards;
}



#endif