package com.example.miles.holdemodds;

import java.util.ArrayList;
import java.util.Arrays;

public class Hand implements Comparable<Hand>{

    int hierarchy;
    double rank;
    int[] otherCardValues;

    public Hand(int inputHierarchy, double inputRank, int[] inputotherCardValues){

        this.hierarchy = inputHierarchy;
        this.rank = inputRank;
        this.otherCardValues = inputotherCardValues;
    }

    public void copyHand(Hand otherHand){
        this.hierarchy = otherHand.hierarchy;
        this.rank = otherHand.rank;
        this.otherCardValues = otherHand.otherCardValues;
    }


    public void setHand(int inputHierarchy, double inputRank, int[] inputotherCardValues){
        this.hierarchy = inputHierarchy;
        this.rank = inputRank;
        this.otherCardValues = inputotherCardValues;
    }

    public String hierarchyToHand(){
        switch (this.hierarchy){
            case 9:
                return "royal flush";
            case 8:
                return "straight flush";
            case 7:
                return "four of a kind";
            case 6:
                return "full house";
            case 5:
                return "flush";
            case 4:
                return "straight";
            case 3:
                return "three of a kind";
            case 2:
                return "two pair";
            case 1:
                return "pair";
            default:
                return "high card";
        }
    }

    // Use hierarchy to get Hand
    public String toString(){
        return this.hierarchyToHand()+" -- RANK: " + this.rank + ", OTHER CARDS: "+Arrays.toString(this.otherCardValues);
    }

    @Override
    public int compareTo(Hand otherHand) {
        if (otherHand == null){
            return 1;
        }
        else if (this.hierarchy > otherHand.hierarchy){
            return 1;
        }
        else if(this.hierarchy < otherHand.hierarchy){
            return -1;
        }
        else{
            if (this.rank > otherHand.rank){
                return 1;
            }
            else if(this.rank < otherHand.rank){
                return -1;
            }
            else{
                for (int i = 0; i < this.otherCardValues.length; i++){

                    if (this.otherCardValues[i] > otherHand.otherCardValues[i]){
                        return 1;
                    }
                    else if (this.otherCardValues[i] < otherHand.otherCardValues[i]){
                        return -1;
                    }
                }
            }
        }
        return 0;
    }

    public static void main(String[] args){
        Hand h1 = new Hand(0, 1.0, new int[]{14,13,11,8,5});
        Hand h2 = new Hand(0, 1.0, new int[]{13,12,11,8,7});
        System.out.println(h1.compareTo(h2));
    }
}
