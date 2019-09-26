package com.example.miles.holdemodds;

import java.text.DecimalFormat;
import java.util.Arrays;
import java.util.HashMap;

public class ThreadTest implements Runnable{

    Thread threadObj;
    double[] winArray;
    double[] tieArray;
    Player[] players;
    Card[] board;

    ThreadTest(HoldEm inputGame) {
        this.threadObj = new Thread(this);
        this.winArray = new double[9];
        this.tieArray = new double[9];
        this.players = inputGame.players;
        this.board = inputGame.board;
    }

    @Override
    public void run() {
        Player[] players = this.makePlayers(this.players);
        Card[] board = this.makeBoard(this.board);
        HoldEm game = new HoldEm(players, board);

        game.getWinPercentages(this.winArray, this.tieArray);

    }

    public void start(){
        threadObj.start();
    }

    // creates a deep copy of player[]
    public Player[] makePlayers(Player[] players){
        Player[] newPlayers = new Player[players.length];

        for (int i = 0; i < players.length; i++){
            newPlayers[i] = new Player(i, new Card[]{null,null});

            for (int j = 0; j < 2; j++){
                if (players[i].hand[j] != null){
                    newPlayers[i].hand[j] = players[i].hand[j];
                }
            }
        }
        return newPlayers;
    }

    // creates a deep copy of board
    private Card[] makeBoard(Card[] board){
        Card[] newBoard = new Card[board.length];
        for (int i = 0; i < board.length; i++){
            if (board[i] != null){
                newBoard[i] = board[i];
            }
        }
        return newBoard;
    }



    public static void main(String args[]) throws InterruptedException {
        Card h1 = new Card(Symbol.ACE, Suit.DIAMONDS);
        Card h2 = new Card(Symbol.TWO, Suit.DIAMONDS);

        Card h3 = new Card(Symbol.QUEEN, Suit.HEARTS);
        Card h4 = new Card(Symbol.TWO, Suit.SPADES);

        Card[] board = new Card[0];//{b1,b2,b3,b4};

        Player p1 = new Player(0,new Card[] {h1, h2});
        Player p2 = new Player(1,new Card[] {h3, h4});
        Player p3 = new Player(2,new Card[] {null, null});
        Player p4 = new Player(3,new Card[] {null, null});
        Player p5 = new Player(4,new Card[] {null, null});
        Player p6 = new Player(5,new Card[] {null, null});
        Player p7 = new Player(6,new Card[] {null, null});
        Player p8 = new Player(7,new Card[] {null, null});
        Player p9 = new Player(8,new Card[] {null, null});

        Player[] players = new Player[] {p1, p2, p3, p4, p5, p6, p7, p8, p9};


        HoldEm game = new HoldEm(players, board);


        long start = System.nanoTime();
        ThreadTest R1 = new ThreadTest(game);
        R1.start();

        ThreadTest R2 = new ThreadTest(game);
        R2.start();

        ThreadTest R3 = new ThreadTest(game);
        R3.start();

        ThreadTest R4 = new ThreadTest(game);
        R4.start();

        ThreadTest R5 = new ThreadTest(game);
        R5.start();

        // Waits until all threads are finished
        R1.threadObj.join();
        R2.threadObj.join();
        R3.threadObj.join();
        R4.threadObj.join();
        R5.threadObj.join();

        long stop = System.nanoTime();
        System.out.println("---------------------------------------------------");
        System.out.println("Execution time: "+(stop-start)/(1000000000.0)+" seconds");

        double[] winPercentages = new double[players.length];
        double[] tiePercentages = new double[players.length];
        DecimalFormat df2 = new DecimalFormat(".####");

        for (int i = 0; i < players.length; i++){
            winPercentages[i] += R1.winArray[i]+R2.winArray[i]+R3.winArray[i]+R4.winArray[i]+R5.winArray[i];
            tiePercentages[i] += R1.tieArray[i]+R2.tieArray[i]+R3.tieArray[i]+R4.tieArray[i]+R5.tieArray[i];
        }

        for (int i = 0; i < players.length; i++){
            winPercentages[i] = Double.valueOf(df2.format(winPercentages[i]/5.0));
            tiePercentages[i] = Double.valueOf(df2.format(tiePercentages[i]/5.0));
        }

        System.out.println(Arrays.toString(winPercentages));
        System.out.println(Arrays.toString(tiePercentages));

    }

}
