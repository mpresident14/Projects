package com.example.miles.holdem_cpp;

import android.content.Context;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.IdRes;
import android.support.constraint.ConstraintLayout;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.PopupWindow;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class WinPercentages extends AppCompatActivity{// implements View.OnClickListener{



    int numPlayers = 2;
    ImageView p1_c1;
    ImageView p1_c2;
    ImageView p2_c1;
    ImageView p2_c2;
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
    Handler handler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_win_percentages);
        p1_c1 = (ImageView) findViewById(R.id.player1_card1);
        p1_c2 = (ImageView) findViewById(R.id.player1_card2);
        p2_c1 = (ImageView) findViewById(R.id.player2_card1);
        p2_c2 = (ImageView) findViewById(R.id.player2_card2);
        b1 = (ImageView) findViewById(R.id.board1);
        b2 = (ImageView) findViewById(R.id.board2);
        b3 = (ImageView) findViewById(R.id.board3);
        b4 = (ImageView) findViewById(R.id.board4);
        b5 = (ImageView) findViewById(R.id.board5);
        usedCards = new ArrayList<String>();
        top = (ConstraintLayout) findViewById(R.id.top);
        calculate = (Button) findViewById(R.id.calculate);

//        p1_c1.setOnClickListener(this);
//        p1_c2.setOnClickListener(this);
//        p2_c1.setOnClickListener(this);
//        p2_c2.setOnClickListener(this);
//        b1.setOnClickListener(this);
//        b2.setOnClickListener(this);
//        b3.setOnClickListener(this);
//        b4.setOnClickListener(this);
//        b5.setOnClickListener(this);

        calculate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                player1 = new Player(0,player1_cards.toArray(new Card[2]));
//                player2 = new Player(1,player2_cards.toArray(new Card[2]));
//                player3 = new Player(2,player3_cards.toArray(new Card[2]));
//                player4 = new Player(3,player4_cards.toArray(new Card[2]));
//                player5 = new Player(4,player5_cards.toArray(new Card[2]));
//                player6 = new Player(5,player6_cards.toArray(new Card[2]));
//                player7 = new Player(6,player7_cards.toArray(new Card[2]));
//                player8 = new Player(7,player8_cards.toArray(new Card[2]));
//                player9 = new Player(8,player9_cards.toArray(new Card[2]));
//                Player[] allPossiblePlayers = new Player[]{player1, player2, player3, player4, player5, player6, player7, player8, player9};
//                final Player[] myPlayers = new Player[numPlayers];
//                System.arraycopy(allPossiblePlayers, 0, myPlayers, 0, numPlayers);
//                Card[] myBoard = board.toArray(new Card[board.size()]);
//
//                final HoldEm game = new HoldEm(myPlayers, myBoard);
//
//                Thread t = new Thread(new Runnable(){
//                    @Override
//                    public void run() {
//                        ThreadTest R1 = new ThreadTest(game);
//                        R1.start();
//
//                        ThreadTest R2 = new ThreadTest(game);
//                        R2.start();
//
//                        ThreadTest R3 = new ThreadTest(game);
//                        R3.start();
//
//                        ThreadTest R4 = new ThreadTest(game);
//                        R4.start();
//
//                        ThreadTest R5 = new ThreadTest(game);
//                        R5.start();
//
//                        // Waits until all threads are finished
//                        try {
//                            R1.threadObj.join();
//                            R2.threadObj.join();
//                            R3.threadObj.join();
//                            R4.threadObj.join();
//                            R5.threadObj.join();
//                        } catch (InterruptedException e) {
//                            e.printStackTrace();
//                        }
//
//                        double[] winPercentages = new double[myPlayers.length];
//                        double[] tiePercentages = new double[myPlayers.length];
//                        DecimalFormat df2 = new DecimalFormat(".####");
//
//                        for (int i = 0; i < myPlayers.length; i++){
//                            winPercentages[i] += R1.winArray[i]+R2.winArray[i]+R3.winArray[i]+R4.winArray[i]+R5.winArray[i];
//                            tiePercentages[i] += R1.tieArray[i]+R2.tieArray[i]+R3.tieArray[i]+R4.tieArray[i]+R5.tieArray[i];
//                        }
//
//                        for (int i = 0; i < myPlayers.length; i++){
//                            winPercentages[i] = Double.valueOf(df2.format(winPercentages[i]/5.0));
//                            tiePercentages[i] = Double.valueOf(df2.format(tiePercentages[i]/5.0));
//                        }
//
//                        Message msg = Message.obtain();
//                        msg.obj = new double[][] {winPercentages, tiePercentages};
//                        handler.sendMessage(msg);
//                    }
//                });
//                t.start();
//
//                handler = new Handler() {
//                    @Override
//                    public void handleMessage(Message msg) {
//                        double[] winPercentages =  ((double[][]) msg.obj)[0];
//                        double[] tiePercentages =  ((double[][]) msg.obj)[1];
//                        Resources res = context.getResources();
//                        String pkg = context.getPackageName();
//
//
//                        TextView winText;
//                        TextView tieText;
//                        int num;
//                        int winID;
//                        int tieID;
//                        for (int i = 0; i < winPercentages.length; i++){
//                            num = i+1;
//                            winID = res.getIdentifier("win"+num,"id",pkg);
//                            tieID = res.getIdentifier("tie"+num,"id",pkg);
//                            winText = (TextView) findViewById(winID);
//                            tieText = (TextView) findViewById(tieID);
//                            winText.setText("Win: "+winPercentages[i]);
//                            tieText.setText("Tie: "+tiePercentages[i]);
//                        }
//                        Toast toast = Toast.makeText(context, "Calculation complete", Toast.LENGTH_SHORT);
//                        toast.show();
//                    }
//                };
            }
        });
    }

//    // ADD OR REMOVE PLAYERS
//    public boolean onCreateOptionsMenu(Menu menu){
//        getMenuInflater().inflate(R.menu.win_menu, menu);
//        return true;
//    }
//
//    public boolean onOptionsItemSelected(MenuItem item){
//        switch(item.getItemId()){
//            case R.id.add_player_button:
//                addPlayer();
//                return true;
//            case R.id.remove_player_button:
//                removePlayer();
//                return true;
//            default:
//                return false;
//        }
//    }
//
//    public boolean addPlayer(){
//        if (numPlayers == 9){
//            return false;
//        }
//        ConstraintLayout top = (ConstraintLayout) findViewById(R.id.top);
//
//        String num = String.valueOf(numPlayers+1);
//        String playerTag = "player"+num;
//        String playerCardTag = playerTag+"_cards";
//
//        LinearLayout player = (LinearLayout) top.findViewWithTag(playerTag);
//        LinearLayout playerCards = (LinearLayout) top.findViewWithTag(playerCardTag);
//
//        Resources res = this.getResources();
//        String pkg = this.getPackageName();
//        int id1 = res.getIdentifier(playerTag+"_card1","id",pkg);
//        int id2 = res.getIdentifier(playerTag+"_card2","id",pkg);
//        int winID = res.getIdentifier("win"+num,"id",pkg);
//        String winTag = "win"+num;
//        int tieID = res.getIdentifier("tie"+num,"id",pkg);
//        String tieTag = "tie"+num;
//
//
//        int cardWidth = p1_c1.getWidth();
//        int cardHeight = p1_c1.getHeight();
//        int padding = findViewById(R.id.player1).getPaddingLeft();
//
//        ImageView card1 = new ImageView(this);
//        ImageView card2 = new ImageView(this);
//        LayoutParams cardDimensions = new LayoutParams(cardWidth, cardHeight);
//
//        TextView win = new TextView(this);
//        TextView tie = new TextView(this);
//        LayoutParams textDimensions = new LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
//
//        card1.setLayoutParams(cardDimensions);
//        card1.setId(id1);
//        card1.setClickable(true);
//        card1.setImageResource(R.drawable.card_back);
//        card1.setOnClickListener(this);
//
//        card2.setLayoutParams(cardDimensions);
//        card2.setId(id2);
//        card2.setClickable(true);
//        card2.setImageResource(R.drawable.card_back);
//        card2.setOnClickListener(this);
//
//        win.setLayoutParams(textDimensions);
//        win.setId(winID);
//        win.setTag(winTag);
//        win.setText(R.string.win);
//        win.setTextColor(ContextCompat.getColor(this, R.color.win_color));
//
//        tie.setLayoutParams(textDimensions);
//        tie.setId(tieID);
//        tie.setTag(tieTag);
//        tie.setText(R.string.tie);
//        tie.setTextColor(ContextCompat.getColor(this, R.color.tie_color));
//
//        player.setPadding(padding,0,padding,0);
//        player.addView(win);
//        player.addView(tie);
//
//        playerCards.addView(card1);
//        playerCards.addView(card2);
//        numPlayers++;
//
//        return true;
//    }
//
//    public boolean removePlayer(){
//        if (numPlayers == 2){
//            return false;
//        }
//
//        String num = String.valueOf(numPlayers);
//        String playerTag = "player"+num;
//        String playerCardTag = playerTag+"_cards";
//
//        LinearLayout player = (LinearLayout) top.findViewWithTag(playerTag);
//        LinearLayout playerCards = (LinearLayout) top.findViewWithTag(playerCardTag);
//
//        Resources res = this.getResources();
//        String pkg = this.getPackageName();
//        int id1 = res.getIdentifier(playerTag+"_card1","id",pkg);
//        int id2 = res.getIdentifier(playerTag+"_card2","id",pkg);
//        int winID = res.getIdentifier("win"+num,"id",pkg);
//        int tieID = res.getIdentifier("tie"+num,"id",pkg);
//
//        ImageView card1 = (ImageView) findViewById(id1);
//        ImageView card2 = (ImageView) findViewById(id2);
//        String prevCardStr1 = (String) card1.getTag();
//        String prevCardStr2 = (String) card2.getTag();
//        usedCards.remove(prevCardStr1);
//        usedCards.remove(prevCardStr2);
//
//        player.removeView(findViewById(winID));
//        player.removeView(findViewById(tieID));
//        playerCards.removeAllViews();
//        playerCards.setPadding(0,0,0,0);
//        numPlayers--;
//
//        return true;
//    }
//
//    // CLICKING ON CARDS
//
//    public void removeValueRB(List<String> usedCardst){
//        String suitStr = (String) suit.getTag();
//        int suitStrLength = suitStr.length();
//        for (String cardStr: usedCards){
//            int cardStrLength = cardStr.length();
//            if (cardStr.substring(cardStrLength-suitStrLength, cardStrLength).equals(suitStr)){
//                String valueStr = cardStr.substring(0,cardStrLength-suitStrLength-1);
//                RadioButton valueButton = (RadioButton) chooseValue.findViewWithTag(valueStr);
//                valueButton.setVisibility(View.INVISIBLE);
//            }
//        }
//    }
//
//    public void replaceValueRB(){
//        for (int i = 0; i < 25; i++){
//            chooseValue.getChildAt(i).setVisibility(View.VISIBLE);
//        }
//    }
//
//    public void removeSuitRB(List<String> usedCards){
//        String valueStr = (String) value.getTag();
//        int valueStrLength = valueStr.length();
//        for (String cardStr: usedCards){
//            int cardStrLength = cardStr.length();
//            if (cardStr.substring(0, valueStrLength).equals(valueStr)){
//                String suitStr = cardStr.substring(valueStrLength+1,cardStrLength);
//                RadioButton suitButton = (RadioButton) chooseSuit.findViewWithTag(suitStr);
//                suitButton.setVisibility(View.INVISIBLE);
//            }
//        }
//    }
//
//    public void replaceSuitRB(){
//        for (int i = 0; i < 4; i++){
//            chooseSuit.getChildAt(i).setVisibility(View.VISIBLE);
//        }
//    }
//
//
//    @Override
//    public void onClick(View v) {

//        if (v instanceof ImageView) {
//            final ImageView chosenCard = (ImageView) v;
//
//            LayoutInflater inflater = (LayoutInflater) getApplicationContext().getSystemService(LAYOUT_INFLATER_SERVICE);
//            ViewGroup container = (ViewGroup) inflater.inflate(R.layout.choose_card_popup, null);
//            popUp = new PopupWindow(container, ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT, true);
//            popUp.showAtLocation(v, Gravity.CENTER,0,0);
//
//            chooseSuit = (RadioGroup) container.findViewById(R.id.choose_suit);
//            chooseValue = (RadioGroup) container.findViewById(R.id.choose_value);
//            chooseUnknownCard = (Button)  container.findViewById(R.id.choose_unknown_card);
//
//            chooseUnknownCard.setOnClickListener(new View.OnClickListener() {
//                @Override
//                public void onClick(View v) {
//                    // Instead, use an int (1-52) to identify each card.
//                    // Use int tag (0-8) for linearlayout and get this using same process (in removeFromCardArrList)
//                    // Then, use these args in Holdem::removeCard(ushort cardNum,  short player_pos)
//                    String prevCardName = (String) chosenCard.getTag();
//                    if (prevCardName != null) {
//                        usedCards.remove(prevCardName);
//
//                        Symbol prevCardSymbol = Symbol.strToSymbol(getSymbolFromCardName(prevCardName));
//                        Suit prevCardSuit = Suit.strToSuit(getSuitFromCardName(prevCardName));
//                        Card prevCard = new Card(prevCardSymbol, prevCardSuit);
//
//                        removeFromCardArrList(chosenCard, prevCard);
//                    }
//
//                    chosenCard.setImageResource(R.drawable.card_back);
//                    chosenCard.setTag(null);
//                    popUp.dismiss();
//                }
//            });
//
//            chooseSuit.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
//                @Override
//                public void onCheckedChanged(RadioGroup group, @IdRes int checkedId) {
//                    suit = (RadioButton) chooseSuit.findViewById(checkedId);
//                    replaceValueRB();
//                    removeValueRB(usedCards);
//                    if (chooseValue.getCheckedRadioButtonId() != -1) {
//                        String prevCardName = (String) chosenCard.getTag();
//                        if (prevCardName != null) {
//                            usedCards.remove(prevCardName);
//
//                            Symbol prevCardSymbol = Symbol.strToSymbol(getSymbolFromCardName(prevCardName));
//                            Suit prevCardSuit = Suit.strToSuit(getSuitFromCardName(prevCardName));
//                            Card prevCard = new Card(prevCardSymbol, prevCardSuit);
//
//                            removeFromCardArrList(chosenCard, prevCard);
//                        }
//
//                        StringBuilder sb = new StringBuilder((String) value.getTag());
//                        sb.append("_");
//                        sb.append((String) suit.getTag());
//                        String newCardName = sb.toString();
//                        int cardID = context.getResources().getIdentifier(newCardName, "drawable", context.getPackageName());
//
//                        chosenCard.setImageResource(cardID);
//                        chosenCard.setTag(newCardName);
//                        usedCards.add(newCardName);
//
//                        Symbol newCardSymbol = Symbol.strToSymbol((String) value.getTag());
//                        Suit newCardSuit = Suit.strToSuit((String) suit.getTag());
//                        addToCardArrList(chosenCard, new Card(newCardSymbol, newCardSuit));
//
//                        popUp.dismiss();
//                    }
//                }
//            });
//
//            chooseValue.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
//                @Override
//                public void onCheckedChanged(RadioGroup group, @IdRes int checkedId) {
//                    value = (RadioButton) chooseValue.findViewById(checkedId);
//                    replaceSuitRB();
//                    removeSuitRB(usedCards);
//                    if (chooseSuit.getCheckedRadioButtonId() != -1) {
//                        String prevCardName = (String) chosenCard.getTag();
//                        if (prevCardName != null) {
//                            usedCards.remove(prevCardName);
//
//                            Symbol prevCardSymbol = Symbol.strToSymbol(getSymbolFromCardName(prevCardName));
//                            Suit prevCardSuit = Suit.strToSuit(getSuitFromCardName(prevCardName));
//                            Card prevCard = new Card(prevCardSymbol, prevCardSuit);
//
//                            removeFromCardArrList(chosenCard, prevCard);
//                        }
//
//                        StringBuilder sb = new StringBuilder((String) value.getTag());
//                        sb.append("_");
//                        sb.append((String) suit.getTag());
//                        String newCardName = sb.toString();
//                        int cardID = context.getResources().getIdentifier(newCardName, "drawable", context.getPackageName());
//
//                        chosenCard.setImageResource(cardID);
//                        chosenCard.setTag(newCardName);
//                        usedCards.add(newCardName);
//
//                        Symbol newCardSymbol = Symbol.strToSymbol((String) value.getTag());
//                        Suit newCardSuit = Suit.strToSuit((String) suit.getTag());
//                        addToCardArrList(chosenCard, new Card(newCardSymbol, newCardSuit));
//
//                        popUp.dismiss();
//                    }
//                }
//            });
//
//
//            container.setOnTouchListener(new View.OnTouchListener() {
//                @Override
//                public boolean onTouch(View v, MotionEvent event) {
//                popUp.dismiss();
//                return false;
//                }
//            });
//
//
//        }
//    }
//
//    private void removeFromCardArrList(ImageView chosenCard, Card prevCard){
//        String parentTag = (String)((View)chosenCard.getParent()).getTag();
//        switch (parentTag){
//            case "player1_cards":
//                player1_cards.remove(prevCard);
//                break;
//            case "player2_cards":
//                player2_cards.remove(prevCard);
//                break;
//            case "player3_cards":
//                player3_cards.remove(prevCard);
//                break;
//            case "player4_cards":
//                player4_cards.remove(prevCard);
//                break;
//            case "player5_cards":
//                player5_cards.remove(prevCard);
//                break;
//            case "player6_cards":
//                player6_cards.remove(prevCard);
//                break;
//            case "player7_cards":
//                player7_cards.remove(prevCard);
//                break;
//            case "player8_cards":
//                player8_cards.remove(prevCard);
//                break;
//            case "player9_cards":
//                player9_cards.remove(prevCard);
//                break;
//            default:
//                board.remove(prevCard);
//                break;
//        }
//    }
//
//    private void addToCardArrList(ImageView chosenCard, Card newCard) {
//        String parentTag = (String) ((View) chosenCard.getParent()).getTag();
//        switch (parentTag) {
//            case "player1_cards":
//                player1_cards.add(newCard);
//                break;
//            case "player2_cards":
//                player2_cards.add(newCard);
//                break;
//            case "player3_cards":
//                player3_cards.add(newCard);
//                break;
//            case "player4_cards":
//                player4_cards.add(newCard);
//                break;
//            case "player5_cards":
//                player5_cards.add(newCard);
//                break;
//            case "player6_cards":
//                player6_cards.add(newCard);
//                break;
//            case "player7_cards":
//                player7_cards.add(newCard);
//                break;
//            case "player8_cards":
//                player8_cards.add(newCard);
//                break;
//            case "player9_cards":
//                player9_cards.add(newCard);
//                break;
//            default:
//                board.add(newCard);
//                break;
//        }
//    }
//
//    public String getSymbolFromCardName(String cardName){
//
//        int i = cardName.indexOf('_');
//        Log.d("cardName",cardName);
//        Log.d("index", Integer.toString(i));
//        return cardName.substring(0, i);
//    }
//
//    public String getSuitFromCardName(String cardName){
//
//        int i = cardName.indexOf('_');
//        return cardName.substring(i+1, cardName.length());
//    }
}


