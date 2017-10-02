package com.example.miles.holdemodds;

/**
 * Created by Miles on 8/11/2017.
 */


public enum Suit {
    CLUBS("clubs"),
    DIAMONDS("diamonds"),
    HEARTS("hearts"),
    SPADES("spades");

    public String name;

    private Suit(String inputName){
        this.name = inputName;
    }

    public static Suit strToSuit(String suitName){

        switch(suitName){
            case "clubs":
                return Suit.CLUBS;
            case "diamonds":
                return Suit.DIAMONDS;
            case "hearts":
                return Suit.HEARTS;
            default:
                return Suit.SPADES;
        }
    }

}

