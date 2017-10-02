package com.example.miles.holdemodds;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.IdRes;
import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumMap;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Random;
import java.util.Set;

import static android.widget.Toast.makeText;

public class HandPercentages extends AppCompatActivity implements View.OnClickListener {

    ImageView p1_c1;
    ImageView p1_c2;
    ImageView b1;
    ImageView b2;
    ImageView b3;
    ImageView b4;
    ImageView b5;
    PopupWindow popUp;
    RadioGroup chooseSuit;
    RadioGroup chooseValue;
    Button chooseUnknownCard;
    RadioButton suit;
    RadioButton value;
    List<String> usedCards;
    Context context = this;
    ConstraintLayout top;
    Button calculate;
    LinearLayout handPercentages;
    RadioGroup chooseNumCardsDealt;
    Handler handler;

    int numCardsToBeShownOnBoard;
    ArrayList<Card> hand;
    ArrayList<Card> board;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hand_percentages);
        p1_c1 = (ImageView) findViewById(R.id.card1);
        p1_c2 = (ImageView) findViewById(R.id.card2);
        b1 = (ImageView) findViewById(R.id.board1);
        b2 = (ImageView) findViewById(R.id.board2);
        b3 = (ImageView) findViewById(R.id.board3);
        b4 = (ImageView) findViewById(R.id.board4);
        b5 = (ImageView) findViewById(R.id.board5);
        usedCards = new ArrayList<String>();
        top = (ConstraintLayout) findViewById(R.id.top);
        calculate = (Button) findViewById(R.id.calculate);
        handPercentages = (LinearLayout) findViewById(R.id.hand_percentages);
        chooseNumCardsDealt = (RadioGroup) findViewById(R.id.choose_num_cards_dealt);
        numCardsToBeShownOnBoard = 5;

        hand = new ArrayList<Card>();
        board = new ArrayList<Card>();

        p1_c1.setOnClickListener(this);
        p1_c2.setOnClickListener(this);
        b1.setOnClickListener(this);
        b2.setOnClickListener(this);
        b3.setOnClickListener(this);
        b4.setOnClickListener(this);
        b5.setOnClickListener(this);

        handler = new Handler(){
            @Override
            public void handleMessage(Message msg){
                double[] percentages = (double[]) msg.obj;
                TextView handType;
                for (int i = 0; i < percentages.length; i++){
                    switch(i){
                        case 0:
                            handType = (TextView) findViewById(R.id.high_card);
                            break;
                        case 1:
                            handType = (TextView) findViewById(R.id.pair);
                            break;
                        case 2:
                            handType = (TextView) findViewById(R.id.two_pair);
                            break;
                        case 3:
                            handType = (TextView) findViewById(R.id.three_of_a_kind);
                            break;
                        case 4:
                            handType = (TextView) findViewById(R.id.straight);
                            break;
                        case 5:
                            handType = (TextView) findViewById(R.id.flush);
                            break;
                        case 6:
                            handType = (TextView) findViewById(R.id.full_house);
                            break;
                        case 7:
                            handType = (TextView) findViewById(R.id.four_of_a_kind);
                            break;
                        case 8:
                            handType = (TextView) findViewById(R.id.straight_flush);
                            break;
                        default:
                            handType = (TextView) findViewById(R.id.royal_flush);
                            break;
                    }
                    StringBuilder text = new StringBuilder((String) handType.getTag());
                    text.append(": ");
                    text.append(Double.toString(percentages[i]));
                    handType.setText(text);
                }
                Toast toast = Toast.makeText(context, "Calculation complete", Toast.LENGTH_SHORT);
                toast.show();
            }
        };

        calculate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Card[] handArray = hand.toArray(new Card[hand.size()]);
                Card[] boardArray = board.toArray(new Card[board.size()]);
                final Player player = new Player(handArray, boardArray);

                final int numCardsToBeDealt = numCardsToBeShownOnBoard - boardArray.length + 2 - handArray.length;
                if (boardArray.length > numCardsToBeShownOnBoard) {
                    Toast toast = Toast.makeText(context, "Too many community cards", Toast.LENGTH_SHORT);
                    toast.show();
                }
                else {

                    Thread t = new Thread(new Runnable() {
                        public void run() {
                            Message msg = Message.obtain();
                            long start = System.nanoTime();
                            msg.obj = player.getPercentages(numCardsToBeDealt);
                            long stop = System.nanoTime();
                            Log.v("Execution time", Double.toString((stop-start)/(1000000000.0)));
                            handler.sendMessage(msg);
                        }
                    });
                    t.start();
                }
                // SHOW CALCULATING GRAPHIC
            }
        });

        chooseNumCardsDealt.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, @IdRes int checkedId) {
                switch(checkedId){
                    case R.id.flop:
                        numCardsToBeShownOnBoard = 3;
                        break;
                    case R.id.turn:
                        numCardsToBeShownOnBoard = 4;
                        break;
                    default:
                        numCardsToBeShownOnBoard = 5;
                        break;
                }
            }
        });
    }

    // CLICKING ON CARDS

    public void removeValueRB(){
        String suitStr = (String) suit.getTag();
        int suitStrLength = suitStr.length();
        for (String cardStr: usedCards){
            int cardStrLength = cardStr.length();
            if (cardStr.substring(cardStrLength-suitStrLength, cardStrLength).equals(suitStr)){
                String valueStr = cardStr.substring(0,cardStrLength-suitStrLength-1);
                RadioButton valueButton = (RadioButton) chooseValue.findViewWithTag(valueStr);
                valueButton.setVisibility(View.INVISIBLE);
            }
        }
    }

    public void replaceValueRB(){
        for (int i = 0; i < 13; i++){
            chooseValue.getChildAt(i).setVisibility(View.VISIBLE);
        }
    }

    public void removeSuitRB(){
        String valueStr = (String) value.getTag();
        int valueStrLength = valueStr.length();
        for (String cardStr: usedCards){
            int cardStrLength = cardStr.length();
            if (cardStr.substring(0, valueStrLength).equals(valueStr)){
                String suitStr = cardStr.substring(valueStrLength+1,cardStrLength);
                RadioButton suitButton = (RadioButton) chooseSuit.findViewWithTag(suitStr);
                suitButton.setVisibility(View.INVISIBLE);
            }
        }
    }

    public void replaceSuitRB(){
        for (int i = 0; i < 4; i++){
            chooseSuit.getChildAt(i).setVisibility(View.VISIBLE);
        }
    }

    @Override
    public void onClick(View v) {

        if (v instanceof ImageView) {
            final ImageView chosenCard = (ImageView) v;

            LayoutInflater inflater = (LayoutInflater) getApplicationContext().getSystemService(LAYOUT_INFLATER_SERVICE);
            ViewGroup container = (ViewGroup) inflater.inflate(R.layout.choose_card_popup, null);
            popUp = new PopupWindow(container, ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT, true);
            popUp.showAtLocation(v,Gravity.CENTER,0,0);

            chooseSuit = (RadioGroup) container.findViewById(R.id.choose_suit);
            chooseValue = (RadioGroup) container.findViewById(R.id.choose_value);
            chooseUnknownCard = (Button)  container.findViewById(R.id.choose_unknown_card);

            chooseUnknownCard.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {

                    // Remove previous card
                    String prevCardName = (String) chosenCard.getTag();
                    if (prevCardName != null) {
                        usedCards.remove(prevCardName);

                        Symbol prevCardSymbol = Symbol.strToSymbol(getSymbolFromCardName(prevCardName));
                        Suit prevCardSuit = Suit.strToSuit(getSuitFromCardName(prevCardName));
                        Card prevCard = new Card(prevCardSymbol, prevCardSuit);

                        if (chosenCard.getParent() == findViewById(R.id.player_cards)) {
                            hand.remove(prevCard);
                        } else if (chosenCard.getParent() == findViewById(R.id.board)) {
                            board.remove(prevCard);
                        }
                    }

                    chosenCard.setImageResource(R.drawable.card_back);
                    chosenCard.setTag(null);
                    popUp.dismiss();
                }
            });

            chooseSuit.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(RadioGroup group, @IdRes int checkedId) {
                    suit = (RadioButton) chooseSuit.findViewById(checkedId);
                    replaceValueRB();
                    removeValueRB();
                    if (chooseValue.getCheckedRadioButtonId() != -1) {

                        // Remove previous card
                        String prevCardName = (String) chosenCard.getTag();


                        if (prevCardName != null) {
                            usedCards.remove(prevCardName);

                            Symbol prevCardSymbol = Symbol.strToSymbol(getSymbolFromCardName(prevCardName));
                            Suit prevCardSuit = Suit.strToSuit(getSuitFromCardName(prevCardName));
                            Card prevCard = new Card(prevCardSymbol, prevCardSuit);

                            if (chosenCard.getParent() == findViewById(R.id.player_cards)) {
                                hand.remove(prevCard);
                            } else if (chosenCard.getParent() == findViewById(R.id.board)) {
                                board.remove(prevCard);
                            }
                        }

                        // Add new card
                        StringBuilder sb = new StringBuilder((String) value.getTag());
                        sb.append("_");
                        sb.append((String) suit.getTag());
                        String newCardName = sb.toString();
                        int cardID = context.getResources().getIdentifier(newCardName, "drawable", context.getPackageName());

                        chosenCard.setImageResource(cardID);
                        chosenCard.setTag(newCardName);
                        usedCards.add(newCardName);

                        Symbol newCardSymbol = Symbol.strToSymbol((String) value.getTag());
                        Suit newCardSuit = Suit.strToSuit((String) suit.getTag());
                        if (chosenCard.getParent()== findViewById(R.id.player_cards)){
                            hand.add(new Card(newCardSymbol, newCardSuit));
                        }
                        else if (chosenCard.getParent()== findViewById(R.id.board)){
                            board.add(new Card(newCardSymbol, newCardSuit));
                        }



                        popUp.dismiss();
                    }
                }
            });

            chooseValue.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(RadioGroup group, @IdRes int checkedId) {
                    value = (RadioButton) chooseValue.findViewById(checkedId);
                    replaceSuitRB();
                    removeSuitRB();
                    if (chooseSuit.getCheckedRadioButtonId() != -1) {

                        // Remove previous card
                        String prevCardName = (String) chosenCard.getTag();
                        if (prevCardName != null) {
                            usedCards.remove(prevCardName);

                            Symbol prevCardSymbol = Symbol.strToSymbol(getSymbolFromCardName(prevCardName));
                            Suit prevCardSuit = Suit.strToSuit(getSuitFromCardName(prevCardName));
                            Card prevCard = new Card(prevCardSymbol, prevCardSuit);

                            if (chosenCard.getParent() == findViewById(R.id.player_cards)) {
                                hand.remove(prevCard);
                            } else if (chosenCard.getParent() == findViewById(R.id.board)) {
                                board.remove(prevCard);
                            }
                        }

                        // Add new card
                        StringBuilder sb = new StringBuilder((String) value.getTag());
                        sb.append("_");
                        sb.append((String) suit.getTag());
                        String newCardName = sb.toString();
                        int cardID = context.getResources().getIdentifier(newCardName, "drawable", context.getPackageName());

                        chosenCard.setImageResource(cardID);
                        chosenCard.setTag(newCardName);
                        usedCards.add(newCardName);

                        // Add to hand/board
                        Symbol cardSymbol = Symbol.strToSymbol((String) value.getTag());
                        Suit cardSuit = Suit.strToSuit((String) suit.getTag());
                        if (chosenCard.getParent()== findViewById(R.id.player_cards)){
                            hand.add(new Card(cardSymbol, cardSuit));
                        }
                        else if (chosenCard.getParent()== findViewById(R.id.board)){
                            board.add(new Card(cardSymbol, cardSuit));
                        }


                        popUp.dismiss();
                    }
                }
            });


            container.setOnTouchListener(new View.OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
                    popUp.dismiss();
                    return false;
                }
            });
        }
    }

    public String getSymbolFromCardName(String cardName){

        int i = cardName.indexOf('_');
        return cardName.substring(0, i);
    }

    public String getSuitFromCardName(String cardName){

        int i = cardName.indexOf('_');
        return cardName.substring(i+1, cardName.length());
    }
}
