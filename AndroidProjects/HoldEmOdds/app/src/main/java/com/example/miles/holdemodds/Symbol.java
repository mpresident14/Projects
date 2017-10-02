package com.example.miles.holdemodds;

/**
 * Created by Miles on 8/11/2017.
 */


public enum Symbol {

    TWO(2),
    THREE(3),
    FOUR(4),
    FIVE(5),
    SIX(6),
    SEVEN(7),
    EIGHT(8),
    NINE(9),
    TEN(10),
    JACK(11),
    QUEEN(12),
    KING(13),
    ACE(14);

    public int number;

    private Symbol(int inputNumber){
        this.number = inputNumber;
    }

    public static Symbol intToSymbol(int n){
        switch(n){
            case 2:
                return Symbol.TWO;
            case 3:
                return Symbol.THREE;
            case 4:
                return Symbol.FOUR;
            case 5:
                return Symbol.FIVE;
            case 6:
                return Symbol.SIX;
            case 7:
                return Symbol.SEVEN;
            case 8:
                return Symbol.EIGHT;
            case 9:
                return Symbol.NINE;
            case 10:
                return Symbol.TEN;
            case 11:
                return Symbol.JACK;
            case 12:
                return Symbol.QUEEN;
            case 13:
                return Symbol.KING;
            default:
                return Symbol.ACE;
        }

    }

    public static Symbol strToSymbol(String n){

        switch(n){
            case "two":
                return Symbol.TWO;
            case "three":
                return Symbol.THREE;
            case "four":
                return Symbol.FOUR;
            case "five":
                return Symbol.FIVE;
            case "six":
                return Symbol.SIX;
            case "seven":
                return Symbol.SEVEN;
            case "eight":
                return Symbol.EIGHT;
            case "nine":
                return Symbol.NINE;
            case "ten":
                return Symbol.TEN;
            case "jack":
                return Symbol.JACK;
            case "queen":
                return Symbol.QUEEN;
            case "king":
                return Symbol.KING;
            default:
                return Symbol.ACE;
        }

    }
}

