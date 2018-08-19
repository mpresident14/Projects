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
import java.util.Arrays;
import java.util.List;
import java.util.stream.IntStream;

public class WinPercentages extends AppCompatActivity implements View.OnClickListener {

    int numPlayers = 2;
    int numCardsOnBoard = 0;
    WinPercentagesBackgroundTask.WinPercentagesListener listener = this::onPercentagesLoaded;
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
    List<Integer> usedCards;
    Context context = this;
    ConstraintLayout top;
    Button calculate;
    Handler handler;
    int[] info;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_win_percentages);

        info = new int[24];
        Arrays.fill(info, -1);

        p1_c1 = (ImageView) findViewById(R.id.player1_card1);
        p1_c2 = (ImageView) findViewById(R.id.player1_card2);
        p2_c1 = (ImageView) findViewById(R.id.player2_card1);
        p2_c2 = (ImageView) findViewById(R.id.player2_card2);
        b1 = (ImageView) findViewById(R.id.board1);
        b2 = (ImageView) findViewById(R.id.board2);
        b3 = (ImageView) findViewById(R.id.board3);
        b4 = (ImageView) findViewById(R.id.board4);
        b5 = (ImageView) findViewById(R.id.board5);
        usedCards = new ArrayList<>();
        top = (ConstraintLayout) findViewById(R.id.top);
        calculate = (Button) findViewById(R.id.calculate);

        p1_c1.setOnClickListener(this);
        p1_c2.setOnClickListener(this);
        p2_c1.setOnClickListener(this);
        p2_c2.setOnClickListener(this);
        b1.setOnClickListener(this);
        b2.setOnClickListener(this);
        b3.setOnClickListener(this);
        b4.setOnClickListener(this);
        b5.setOnClickListener(this);

        p1_c1.setTag(-1);
        p1_c2.setTag(-1);
        p2_c1.setTag(-1);
        p2_c2.setTag(-1);
        b1.setTag(-1);
        b2.setTag(-1);
        b3.setTag(-1);
        b4.setTag(-1);
        b5.setTag(-1);

        calculate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                info[0] = numPlayers;
                Log.v("MyTest", Arrays.toString(info));

                new WinPercentagesBackgroundTask(info).setListener(listener).execute();
        }
    });
}

    // ADD OR REMOVE PLAYERS
    public boolean onCreateOptionsMenu(Menu menu){
        getMenuInflater().inflate(R.menu.win_menu, menu);
        return true;
    }

    public boolean onOptionsItemSelected(MenuItem item){
        switch(item.getItemId()){
            case R.id.add_player_button:
                addPlayer();
                return true;
            case R.id.remove_player_button:
                removePlayer();
                return true;
            default:
                return false;
        }
    }

    public boolean addPlayer(){
        if (numPlayers == 9){
            return false;
        }
        ConstraintLayout top = (ConstraintLayout) findViewById(R.id.top);

        String num = String.valueOf(numPlayers+1);
        String playerTag = "player"+num;
        String playerCardTag = playerTag+"_cards";

        LinearLayout player = (LinearLayout) top.findViewWithTag(playerTag);
        LinearLayout playerCards = (LinearLayout) top.findViewWithTag(playerCardTag);

        Resources res = this.getResources();
        String pkg = this.getPackageName();
        int id1 = res.getIdentifier(playerTag+"_card1","id",pkg);
        int id2 = res.getIdentifier(playerTag+"_card2","id",pkg);
        int winID = res.getIdentifier("win"+num,"id",pkg);
        String winTag = "win"+num;
        int tieID = res.getIdentifier("tie"+num,"id",pkg);
        String tieTag = "tie"+num;


        int cardWidth = p1_c1.getWidth();
        int cardHeight = p1_c1.getHeight();
        int padding = findViewById(R.id.player1).getPaddingLeft();

        ImageView card1 = new ImageView(this);
        ImageView card2 = new ImageView(this);
        LayoutParams cardDimensions = new LayoutParams(cardWidth, cardHeight);

        TextView win = new TextView(this);
        TextView tie = new TextView(this);
        LayoutParams textDimensions = new LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);

        card1.setLayoutParams(cardDimensions);
        card1.setId(id1);
        card1.setClickable(true);
        card1.setImageResource(R.drawable.card_back);
        card1.setOnClickListener(this);
        card1.setTag(-1);

        card2.setLayoutParams(cardDimensions);
        card2.setId(id2);
        card2.setClickable(true);
        card2.setImageResource(R.drawable.card_back);
        card2.setOnClickListener(this);
        card2.setTag(-1);

        win.setLayoutParams(textDimensions);
        win.setId(winID);
        win.setTag(winTag);
        win.setText(R.string.win);
        win.setTextColor(ContextCompat.getColor(this, R.color.win_color));

        tie.setLayoutParams(textDimensions);
        tie.setId(tieID);
        tie.setTag(tieTag);
        tie.setText(R.string.tie);
        tie.setTextColor(ContextCompat.getColor(this, R.color.tie_color));

        player.setPadding(padding,0,padding,0);
        player.addView(win);
        player.addView(tie);

        playerCards.addView(card1);
        playerCards.addView(card2);
        numPlayers++;

        return true;
    }

    public boolean removePlayer(){
        if (numPlayers == 2){
            return false;
        }

        String num = String.valueOf(numPlayers);
        String playerTag = "player"+num;
        String playerCardTag = playerTag+"_cards";

        LinearLayout player = (LinearLayout) top.findViewWithTag(playerTag);
        LinearLayout playerCards = (LinearLayout) top.findViewWithTag(playerCardTag);

        Resources res = this.getResources();
        String pkg = this.getPackageName();
        int id1 = res.getIdentifier(playerTag+"_card1","id",pkg);
        int id2 = res.getIdentifier(playerTag+"_card2","id",pkg);
        int winID = res.getIdentifier("win"+num,"id",pkg);
        int tieID = res.getIdentifier("tie"+num,"id",pkg);

        ImageView card1 = (ImageView) findViewById(id1);
        ImageView card2 = (ImageView) findViewById(id2);
        int prevCardNum1 = (int) card1.getTag();
        int prevCardNum2 = (int) card2.getTag();
        usedCards.remove((Integer) prevCardNum1);
        usedCards.remove((Integer) prevCardNum2);
        setPlayerInfo(card1, -1);
        setPlayerInfo(card2, -1);

        player.removeView(findViewById(winID));
        player.removeView(findViewById(tieID));
        playerCards.removeAllViews();
        playerCards.setPadding(0,0,0,0);
        numPlayers--;

        return true;
    }

    // CLICKING ON CARDS

    // When we click on a suit RB, the used values of that suit should disappear
    public void removeValueRB(){
        int suitNum = Integer.parseInt((String) suit.getTag());
        String valueNum;
        // Loop through card numbers and find the values of that suit that have been used and make their buttons invisible
        for (int i = 0; i < usedCards.size(); ++i){
            if (usedCards.get(i) / 13 == suitNum){
                valueNum = Integer.toString(usedCards.get(i) % 13);
                chooseValue.findViewWithTag(valueNum).setVisibility(View.INVISIBLE);
            }
        }
    }

    public void replaceValueRB(){
        for (int i = 0; i < 13; i++){
            chooseValue.getChildAt(i).setVisibility(View.VISIBLE);
        }
    }

    public void removeSuitRB(){
        int valueNum = Integer.parseInt((String) value.getTag());
        String suitNum;
        for (int i = 0; i < usedCards.size(); ++i){
            if (usedCards.get(i) % 13 == valueNum){
                suitNum = Integer.toString(usedCards.get(i) / 13);
                chooseValue.findViewWithTag(suitNum).setVisibility(View.INVISIBLE);
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
            popUp.showAtLocation(v, Gravity.CENTER,0,0);

            chooseSuit = (RadioGroup) container.findViewById(R.id.choose_suit);
            chooseValue = (RadioGroup) container.findViewById(R.id.choose_value);
            chooseUnknownCard = (Button)  container.findViewById(R.id.choose_unknown_card);

            chooseUnknownCard.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    // Instead, use an int (1-52) to identify each card.
                    // Use int tag (0-8) for linearlayout and get this using same process (in removeFromCardArrList)
                    // Then, use these args in Holdem::removeCard(ushort cardNum,  short player_pos)
                    int prevCardNum = (int) chosenCard.getTag();
                    if (prevCardNum != -1) {
                        usedCards.remove((Integer) prevCardNum);
                        if (chosenCard.getParent() == findViewById(R.id.board)){
                            // Update index in info
                            info[numCardsOnBoard] = -1;
                            // Update number of board cards if card was on the board
                            --numCardsOnBoard;
                        }
                        else {
                            setPlayerInfo(chosenCard, -1);
                        }
                    }

                    chosenCard.setImageResource(R.drawable.card_back);
                    chosenCard.setTag(-1);
                    popUp.dismiss();
                }
            });

            chooseSuit.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(RadioGroup group, @IdRes int checkedId) {
                    suit = chooseSuit.findViewById(checkedId);
                    replaceValueRB();
                    removeValueRB();
                    if (chooseValue.getCheckedRadioButtonId() != -1) { // This checks if RadioButton in value group is checked
                        // Construct the tag (cardNum) and id
                        int cardNum = 13 * Integer.parseInt((String)suit.getTag()) + Integer.parseInt((String) value.getTag());
                        int cardID = context.getResources().getIdentifier("c"+Integer.toString(cardNum), "drawable", context.getPackageName());

                        int prevCardNum = (int) chosenCard.getTag();
                        if (prevCardNum != -1) {
                            usedCards.remove(Integer.valueOf(prevCardNum));
                        }

                        if (chosenCard.getParent() == findViewById(R.id.board)) {
                            if (prevCardNum == -1) {
                                // Update number of board cards if card was previously unknown
                                ++numCardsOnBoard;
                            }
                            // Update index in info
                            info[numCardsOnBoard] = cardNum;
                        }
                        else {
                            setPlayerInfo(chosenCard, cardNum);
                        }

                        chosenCard.setImageResource(cardID);
                        chosenCard.setTag(cardNum);

                        usedCards.add(cardNum);

                        popUp.dismiss();
                    }
                }
            });

            chooseValue.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(RadioGroup group, @IdRes int checkedId) {
                    value = chooseValue.findViewById(checkedId);
                    replaceSuitRB();
                    removeSuitRB();
                    if (chooseSuit.getCheckedRadioButtonId() != -1) {

                        // Construct the tag (cardNum) and id
                        int cardNum = 13 * Integer.parseInt((String)suit.getTag()) + Integer.parseInt((String) value.getTag());
                        int cardID = context.getResources().getIdentifier("c"+Integer.toString(cardNum), "drawable", context.getPackageName());

                        int prevCardNum = (int) chosenCard.getTag();
                        if (prevCardNum != -1) {
                            usedCards.remove(Integer.valueOf(prevCardNum));
                        }

                        if (chosenCard.getParent() == findViewById(R.id.board)) {
                            if (prevCardNum == -1) {
                                // Update number of board cards if card was previously unknown
                                ++numCardsOnBoard;
                            }
                            // Update index in info
                            info[numCardsOnBoard] = cardNum;
                        }
                        else {
                            setPlayerInfo(chosenCard, cardNum);
                        }

                        chosenCard.setImageResource(cardID);
                        chosenCard.setTag(cardNum);

                        usedCards.add(cardNum);

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

    void onPercentagesLoaded(double[] winPercentages, double[] tiePercentages) {
        Resources res = context.getResources();
        String pkg = context.getPackageName();
        DecimalFormat df = new DecimalFormat("##0.00%");

        TextView winText;
        TextView tieText;
        int num;
        int winID;
        int tieID;
        for (int i = 0; i < winPercentages.length; i++){
            num = i+1;
            winID = res.getIdentifier("win"+num,"id",pkg);
            tieID = res.getIdentifier("tie"+num,"id",pkg);
            winText = (TextView) findViewById(winID);
            tieText = (TextView) findViewById(tieID);
            winText.setText("Win: " + df.format(winPercentages[i]));
            tieText.setText("Tie: " + df.format(tiePercentages[i]));
        }
        Toast toast = Toast.makeText(context, "Calculation complete", Toast.LENGTH_SHORT);
        toast.show();
    }

    private void setPlayerInfo(ImageView v, int cardNum) {
        String id = v.getResources().getResourceEntryName(v.getId());
        // Player num comes right after "player" in the tag, -1 to convert to zero index
        int playerNum = Character.getNumericValue(id.charAt(6)) - 1;
        // Card position is last char in tag, -1 to convert to zero index
        int cardPosition = Character.getNumericValue((id.charAt(id.length() - 1))) - 1;
        info[6 + 2*playerNum + cardPosition] = cardNum;
    }
}


