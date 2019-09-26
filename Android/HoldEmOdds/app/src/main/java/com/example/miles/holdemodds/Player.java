package com.example.miles.holdemodds;


/**
 * Created by Miles on 8/11/2017.
 */



import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumMap;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Random;
import java.util.Set;

public class Player {

    int position;
    Card[] board;
    Card[] hand;
    Suit[] suitArr;
    Symbol[] symbolArr;
    int zeroIndex;
    Hand flushesHand;
    Hand counterHand;
    Hand straightHand;




    // Constructor
    public Player(int inputPosition, Card[] inputHand){
        this.position = inputPosition;
        this.hand = inputHand;
        this.suitArr = Suit.values();
        this.symbolArr = Symbol.values();
        this.zeroIndex = 0;
        this.flushesHand = new Hand(0,0,new int[0]);
        this.counterHand = new Hand(0,0,new int[0]);
        this.straightHand = new Hand(0,0,new int[0]);
    }

    public Player(Card[] inputHand, Card[] inputBoard){
        this.hand = inputHand;
        this.board = inputBoard;
        this.suitArr = Suit.values();
        this.symbolArr = Symbol.values();
        this.zeroIndex = 0;
        this.flushesHand = new Hand(0,0,new int[0]);
        this.counterHand = new Hand(0,0,new int[0]);
        this.straightHand = new Hand(0,0,new int[0]);


    }

    //-----------------------------------------------------------------------------------------------
    public void getValueCount(EnumMap<Symbol, Integer> valueCount, Card[] entireHand){

        valueCount.clear();
        for(int i = 0; i < entireHand.length; i++){
            Symbol key = entireHand[i].symbol;

            if (!valueCount.containsKey(key)){
                valueCount.put(key, 1);
            }
            else{
                valueCount.put(key, valueCount.get(key)+1);
            }
        }
    }

    public void getSuitCount(EnumMap<Suit, Integer> suitCount, Card[] entireHand){

        suitCount.clear();
        for (int i = 0; i < this.suitArr.length; i++){
            suitCount.put(this.suitArr[i], 0);
        }

        for(int i = 0; i < entireHand.length; i++){
            Suit key = entireHand[i].suit;
            suitCount.put(key, suitCount.get(key)+1);
        }
    }

    public void getValueList(int[] valueList, EnumMap<Symbol, Integer> valueCount){

        for (int i = 0; i < valueList.length; i++){
            valueList[i] = 0;
        }

        int j = 0;
        for (int i = 0; i < this.symbolArr.length; i++){
            if (valueCount.get(this.symbolArr[i]) != null){
                valueList[j] = this.symbolArr[i].number;
                j++;
            }
        }
        this.zeroIndex = j;
    }


    //***************************************************************************************

    //Other Helpers

    public static int combo(int n, int k){
        float answer = 1;
        while (k != 0){
            answer *= (n-k+1)*1.0/k;
            k -= 1;
        }
        return Math.round(answer);
    }

    public static ArrayList<Card> getFullDeck(){
        ArrayList<Card> deck = new ArrayList<Card>(Arrays.asList(new Card[52]));
        int i = 0;

        // first suit not getting chosen as often changes percentages
        for(Suit suit: Suit.values()){
            for (Symbol symbol: Symbol.values()){
                Card card = new Card(symbol, suit);
                deck.set(i, card);
                i++;
            }
        }
        return deck;
    }

    //***************************************************************************************

    // Booleans for different hands

    private boolean isStraight(int[] valueList, EnumMap<Symbol, Integer> valueCount){
        this.getValueList(valueList, valueCount);

        int size = this.zeroIndex;
        for(int i = 0; i < size-4; i++){
            if ((valueList[i+4] - valueList[i]) + (valueList[i+3] -
                    valueList[i+1]) + (valueList[i+2] - valueList[i]) == 8){

                return true;
            }

        }
        if ((valueList[this.zeroIndex-1]-valueList[0] == 12) && valueList[1] == 3 &&
                valueList[2]==4 && valueList[3] == 5){
            return true;
        }

        return false;
    }

    private int isCounterHand(EnumMap<Symbol, Integer> valueCount){
        int threeCount = 0;
        int pairCount = 0;

        for (int i = 0; i < this.symbolArr.length; i++){
            if (valueCount.get(this.symbolArr[i]) != null){
                int value = valueCount.get(this.symbolArr[i]);

                if (value == 4){
                    return 7;
                }
                else if (value == 3){
                    threeCount++;
                }
                else if (value == 2){
                    pairCount++;
                }
            }
        }

        if ((threeCount == 1 && pairCount > 0) || (threeCount == 2)){
            return 6;
        }
        else if (threeCount == 1){
            return 3;
        }
        else if (pairCount >= 2){
            return 2;
        }
        else if (pairCount == 1){
            return 1;
        }
        else{
            return 0;
        }
    }


    private int isFlush(EnumMap<Suit, Integer> suitCount, Card[] entireHand, int[] valueList){
        Suit maxSuit = null;
        int numSuits = 0;
        for (int i = 0; i < this.suitArr.length; i++){
            Suit suit = this.suitArr[i];
            numSuits = suitCount.get(suit);
            if (numSuits >= 5){
                maxSuit = suit;
                break;
            }
        }

        if (maxSuit == null){
            return 0;
        }

        for (int i = 0; i < valueList.length; i++){
            valueList[i] = 0;
        }

        int j = 0;

        for (int i = 0; i < entireHand.length; i++){

            Card card = entireHand[i];
            if (card.suit.equals(maxSuit)){
                valueList[j] = card.value;
                j++;
            }


        }
        Arrays.sort(valueList);

        boolean answer = false;
        int start = valueList.length - j;

        for(int i = start; i < valueList.length-4; i++){
            if ((valueList[i+4] - valueList[i]) + (valueList[i+3] -
                    valueList[i+1]) + (valueList[i+2] - valueList[i]) == 8){

                answer = true;
            }

        }
        if ((valueList[valueList.length-1]-valueList[start] == 12) && valueList[start+1] == 3 &&
                valueList[start+2]==4 && valueList[start+3] == 5){

            answer = true;
        }

        if (answer && valueList[valueList.length-1] == 14 && valueList[valueList.length-5] == 10){		// valueList can have other cards of same suit

            return 9;
        }
        else if (answer){
            return 8;
        }
        else{
            return 5;
        }

    }

    //***************************************************************************************

    // getBestHandType()
    private int getBestHandType(EnumMap<Symbol, Integer> valueCount, EnumMap<Suit, Integer> suitCount,
                                int[] valueList, Card[] entireHand){

        int flushes = this.isFlush(suitCount, entireHand, valueList);
        int counters = this.isCounterHand(valueCount);

        if (flushes >= 8){
            return flushes;
        }
        else if (counters >= 6){
            return counters;
        }
        else if (flushes == 5){
            return 5;
        }
        else if (this.isStraight(valueList, valueCount)){
            return 4;
        }
        else{
            return counters;
        }

    }

    //***************************************************************************************

    // Get hands



    private int getStraight(EnumMap<Symbol, Integer> valueCount, int[] valueList){

        int rank = 0;
        this.getValueList(valueList, valueCount);
        int size = this.zeroIndex;
        for(int i = 0; i < size-4; i++){
            if ((valueList[i+4] - valueList[i]) + (valueList[i+3] -
                    valueList[i+1]) + (valueList[i+2] - valueList[i]) == 8){

                rank = valueList[i+4];
                this.straightHand.setHand(4, rank, new int[0]);
                return 4;
            }
        }


        if ((valueList[this.zeroIndex-1]-valueList[0] == 12) && valueList[1] == 3 &&
                valueList[2]==4 && valueList[3] == 5){
            this.straightHand.setHand(4, 5, new int[0]);
            return 4;
        }
        else{
            this.straightHand.setHand(0, 0, new int[0]);
        }

        return -1;
    }


    private int getFlush(EnumMap<Suit, Integer> suitCount, Card[] entireHand, int[] valueList){

        Suit maxSuit = null;
        int numSuits = 0;
        for (Suit key: suitCount.keySet()){
            numSuits = suitCount.get(key);
            if (numSuits >= 5){
                maxSuit = key;
                break;
            }
        }

        if (maxSuit == null){
            this.flushesHand.setHand(0, 0, new int[0]);
            return -1;
        }

        for (int i = 0; i < valueList.length; i++){
            valueList[i] = 0;
        }

        int j = 0;

        for (int i = 0; i < entireHand.length; i++){

            Card card = entireHand[i];
            if (card.suit.equals(maxSuit)){
                valueList[j] = card.value;
                j++;
            }


        }
        Arrays.sort(valueList);

        int size = valueList.length;
        int sfRank = 0;
        int start = valueList.length - j;
        boolean answer = false;


        if ((valueList[valueList.length-1]-valueList[start] == 12) && valueList[start+1] == 3 &&
                valueList[start+2]==4 && valueList[start+3] == 5){

            sfRank = 5;
            answer = true;
        }


        for(int i = start; i < valueList.length-4; i++){
            if ((valueList[i+4] - valueList[i]) + (valueList[i+3] -
                    valueList[i+1]) + (valueList[i+2] - valueList[i]) == 8){

                sfRank = valueList[i];
                answer = true;
            }
        }

        if (answer){
            if(sfRank == 14){
                this.flushesHand.setHand(9, 14, new int[0]);
                return 14;
            }
            else{
                this.flushesHand.setHand(8, sfRank, new int[0]);
                return 8;
            }
        }

        else{

            double flushRank = valueList[size-1] + valueList[size-2]/10.0 + valueList[size-3]/100.0 +
                    valueList[size-4]/1000.0 + valueList[size-5]/10000.0;
            this.flushesHand.setHand(5, flushRank, new int[0]);
            return 5;
        }
    }

    private int getCounterHands(EnumMap<Symbol, Integer> valueCount, int[] valueList){
        this.getValueList(valueList, valueCount);
        int size = this.zeroIndex;
        int threeRank = 0;
        int twoRank1 = 0;
        int twoRank2 = 0;

        int[] otherCardValues = new int[5];
        int k = 0;


        for (int i = size-1; i >= 0; i--){
            int num = valueList[i];
            int count = valueCount.get(Symbol.intToSymbol(num));

            if (count == 4){
                int[] others = new int[1];
                if (i == size-1){
                    others[0] = valueList[i-1];
                }
                else{
                    others[0] = valueList[size-1];
                }
                this.counterHand.setHand(7, num, others);
                return 7;
            }

            else if (count == 3){
                if (threeRank == 0){
                    threeRank = num;
                }
                else if (twoRank1 == 0){
                    twoRank1 = num;
                }
                else if(twoRank2 == 0){
                    twoRank2 = num;
                }
            }

            else if (count == 2){
                if (twoRank1 == 0){
                    twoRank1 = num;
                }
                else if(twoRank2 == 0){
                    twoRank2 = num;
                }
            }

            else{
                if (k < 5){
                    otherCardValues[k] = num;
                    k++;
                }
            }

        }


        if (threeRank != 0){
            if (twoRank1 != 0){
                double rank = threeRank + twoRank1/10.0;
                this.counterHand.setHand(6, rank, new int[0]);
                return 6;
            }

            else{

                int[] others = new int[] {otherCardValues[0], otherCardValues[1]};
                this.counterHand.setHand(3, threeRank, others);
                return 3;
            }
        }
        else if (twoRank1 != 0){
            if (twoRank2 != 0){
                int[] others = new int[] {otherCardValues[0]};
                double rank = twoRank1 + twoRank2/10.0;
                this.counterHand.setHand(2, rank, others);
                return 2;
            }
            else{
                int[] others = new int[] {otherCardValues[0], otherCardValues[1], otherCardValues[2]};
                this.counterHand.setHand(1, twoRank1, others);
                return 1;
            }
        }
        else{
            this.counterHand.setHand(0, 1, otherCardValues);
            return 0;
        }


    }

    public void getHand(Hand myHand, EnumMap<Symbol, Integer> valueCount, EnumMap<Suit, Integer> suitCount, int[] valueList,
                        Card[] entireHand){

        int best = 0;
        int flushes = this.getFlush(suitCount, entireHand, valueList);			// shouldn't calculate all 3 every time
        int counters = this.getCounterHands(valueCount, valueList);
        int straight = this.getStraight(valueCount, valueList);

        if (flushes > best){
            best = flushes;
        }
        if (counters > best){
            best = counters;
        }
        if (straight > best){
            best = straight;
        }

        if (best == flushes){
            myHand.copyHand(this.flushesHand);
        }
        else if (best == straight){
            myHand.copyHand(this.straightHand);
        }
        else{
            myHand.copyHand(this.counterHand);
        }
    }


    //***************************************************************************************

    // toString() for board
    public String toString(){
        ArrayList<String> allCards = new ArrayList<String>();
        for (Card card: this.hand){
            allCards.add(card.symbol+" "+card.suit);
        }
        return allCards.toString();
    }

    public static String printCardArray(Card[] cards){
        ArrayList<String> allCards = new ArrayList<String>();
        for (Card card: cards){
            allCards.add(card.symbol+" "+card.suit);
        }
        return allCards.toString();
    }
    //***************************************************************************************


    private double[] getCombosHelper(EnumMap<Symbol, Integer> valueCount, EnumMap<Suit, Integer> suitCount, int[] valueList,
                                     Card[] entireHand, Card[] deck, Card[] group, int start, int stop, int groupIndex, int k, double[] handCounts){
        if (groupIndex == k){
            int handSize = this.hand.length;
            int boardSize = this.board.length;

            System.arraycopy(group, 0, entireHand, 0, k);
            if (this.board.length != 0){
                System.arraycopy(this.board, 0, entireHand, k, boardSize);
            }

            System.arraycopy(this.hand, 0, entireHand, k+boardSize, handSize);

            // Create valueCount, suitCount, valueList
            this.getValueCount(valueCount, entireHand);
            this.getSuitCount(suitCount, entireHand);


            int bestHand = this.getBestHandType(valueCount, suitCount, valueList, entireHand);
            handCounts[bestHand] += 1;

            return handCounts;

        }
        for (int i=start;i<=stop-k+groupIndex ;i++){ 	//	k-groupIndex: amount of space left in group
            // 	(stop - i) needs to be able to fill up group (>= k-groupIndex)
            // 	move i to other side
            group[groupIndex] = deck[i];
            getCombosHelper(valueCount, suitCount, valueList, entireHand, deck,group,i+1,stop,groupIndex+1,k, handCounts);		// move to next value of arr														// move to next value of group
        }
        return handCounts;
    }

    private double[] getCombos(int numCardsToBeDealt){

        double[] handCounts = new double[10];
        Card[] group = new Card[numCardsToBeDealt];

        ArrayList<Card> deck = Player.getFullDeck();
        deck.removeAll(new ArrayList<Card>(Arrays.asList(this.hand)));
        deck.removeAll(new ArrayList<Card>(Arrays.asList(this.board)));
        int stop = deck.size();
        Card[] deckArray = deck.toArray(new Card[stop]);

        EnumMap<Symbol, Integer> valueCount = new EnumMap<Symbol, Integer>(Symbol.class);
        EnumMap<Suit, Integer> suitCount = new EnumMap<Suit, Integer>(Suit.class);
        int[] valueList = new int[7];
        Card[] entireHand = new Card[numCardsToBeDealt+this.hand.length+this.board.length];

        return getCombosHelper(valueCount, suitCount, valueList, entireHand, deckArray,group,0,stop,0, numCardsToBeDealt, handCounts);

    }

    // handSize <= 2, boardSize + numCardsToBeDealt <= 5
    public double[] getPercentages(int numCardsToBeDealt){
        double[] percentages = getCombos(numCardsToBeDealt);
        int possibleCombos = Player.combo(52-this.board.length-this.hand.length, numCardsToBeDealt);
        DecimalFormat df2 = new DecimalFormat(".#######");

        for (int i = 0; i < percentages.length; i++){
            percentages[i] = Double.valueOf(df2.format(percentages[i]/possibleCombos));
        }

        return percentages;
    }

//---------------------------------------------------------------------------------------------------------------------------------------

    // empty board probabilities (deal 5 cards))
    private boolean isSuited(){
        if (this.hand[0].suit.equals(this.hand[1].suit)){
            return true;
        }
        return false;
    }

    private int numFaces(){
        List<Integer> list = Arrays.asList(10,11,12,13,14);
        if (list.contains(this.hand[0].value)){
            if (list.contains(this.hand[1].value)){
                return 2;
            }
            return 1;
        }

        else if (list.contains(this.hand[1].value)){
            if (list.contains(this.hand[0].value)){
                return 2;
            }
            return 1;
        }
        return 0;

    }

    // least to greatest
    public void orderHand(){
        if (this.hand[0].value > this.hand[1].value){
            Card temp = this.hand[0];
            this.hand[0] = this.hand[1];
            this.hand[1] = temp;
        }
    }


    public int rfCount(){
        int faces = this.numFaces();
        if (faces == 2){
            if(this.isSuited()){
                return 1084;
            }
            return 94;
        }
        else if(faces == 1){
            return 49;
        }
        return 4;
    }

    // suited connectors only
    public int sfCount(){
        this.orderHand();
        int a = 1081;
        int b = 46;
        int c = 0;

        int aCoef = 0;
        int bCoef = 0;

        if (this.hand[1].value > 11){
            aCoef = 15 - this.hand[1].value;
        }
        else if (this.hand[0].value < 4){
            aCoef = this.hand[0].value;
        }
        else{
            aCoef = 4;
        }

        bCoef = aCoef - 1;

        if (this.hand[1].value > 8){
            c = this.hand[1].value - 7;
        }
        else if (this.hand[1].value < 8){
            c = 9-this.hand[1].value;
        }
        else{
            c = 2;
        }

        if(this.hand[1].value == 14){
            aCoef = 1;
            bCoef = -1;
            c = 5;
        }

        return aCoef*a - bCoef*b + c + 30 - this.rfCount();
    }

//---------------------------------------------------------------------------------------------------------------------------------------

    public static void main(String[] args) {
        Card h1 = new Card(Symbol.NINE, Suit.DIAMONDS);
        Card h2 = new Card(Symbol.TWO, Suit.DIAMONDS);

        Card h3 = new Card(Symbol.QUEEN, Suit.DIAMONDS);
        Card h4 = new Card(Symbol.KING, Suit.DIAMONDS);

        Card b1 = new Card(Symbol.EIGHT, Suit.DIAMONDS);
        Card b2 = new Card(Symbol.FIVE, Suit.DIAMONDS);
        Card b3 = new Card(Symbol.THREE, Suit.DIAMONDS);
        Card b4 = new Card(Symbol.JACK, Suit.CLUBS);
        Card b5 = new Card(Symbol.KING, Suit.SPADES);

        Card[] hand1 = new Card[] {h1,h2};
        Card[] hand2 = new Card[] {h3,h4};
        Card[] board = new Card[] {b1,b2,b3,b4,b5};

        Player p1 = new Player(hand1, board);
        Player p2 = new Player(hand2, board);

        Hand c1 = new Hand(0,0,new int[0]);
        Hand c2 = new Hand(0,0,new int[0]);


        Card[] entireHand1 = new Card[] {h1,h2,b1,b2,b3,b4,b5};
        Card[] entireHand2 = new Card[] {h3,h4,b1,b2,b3,b4,b5};
        EnumMap<Symbol, Integer> valueCount1 = new EnumMap<Symbol, Integer>(Symbol.class);
        EnumMap<Symbol, Integer> valueCount2 = new EnumMap<Symbol, Integer>(Symbol.class);
        EnumMap<Suit, Integer> suitCount1 = new EnumMap<Suit, Integer>(Suit.class);
        EnumMap<Suit, Integer> suitCount2 = new EnumMap<Suit, Integer>(Suit.class);
        int[] valueList1 = new int[7];
        int[] valueList2 = new int[7];

        p1.getValueCount(valueCount1, entireHand1);
        p2.getValueCount(valueCount2, entireHand2);
        p1.getSuitCount(suitCount1, entireHand1);
        p2.getSuitCount(suitCount2, entireHand2);
        p1.getValueList(valueList1, valueCount1);
        p2.getValueList(valueList2, valueCount2);


        p1.getHand(c1, valueCount1, suitCount1, valueList1, entireHand1);
        p2.getHand(c2, valueCount2, suitCount2, valueList2, entireHand2);


        System.out.println(c1.compareTo(c2));
//        long start = System.nanoTime();
//        long stop = System.nanoTime();
//
//        System.out.println("Execution time: " +(stop-start)/(1000000000.0)+" seconds");
    }
}


