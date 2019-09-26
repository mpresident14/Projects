package com.example.miles.holdem_cpp;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.IdRes;
import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.PopupWindow;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.ref.WeakReference;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class HandPercentages extends AppCompatActivity{// implements View.OnClickListener {

    // Load "native-lib library
    static{
        System.loadLibrary("native-lib");
    }

    ImageView p1_c1;
    ImageView p1_c2;
    ImageView b1;
    ImageView b2;
    ImageView b3;
    ImageView b4;
    ImageView b5;
    ConstraintLayout top;
    PopupWindow popUp;
    RadioGroup chooseSuit;
    RadioGroup chooseValue;
    Button chooseUnknownCard;
    RadioButton suit;
    RadioButton value;
    Context context = this;
    Button calculate;
    RadioGroup chooseFlopTurnRiver;
    MyHandler handler;

    int numCardsToBeShownOnBoard;
    int numCardsOnBoard;
    List<Integer> cards;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hand_percentages);
        p1_c1 = findViewById(R.id.card1);
        p1_c2 = findViewById(R.id.card2);
        b1 = findViewById(R.id.board1);
        b2 = findViewById(R.id.board2);
        b3 = findViewById(R.id.board3);
        b4 = findViewById(R.id.board4);
        b5 = findViewById(R.id.board5);
        top = findViewById(R.id.top);

        cards = new ArrayList<>(7); // TODO: Add in the -1s afterward

        calculate = findViewById(R.id.calculate);
        chooseFlopTurnRiver = findViewById(R.id.choose_num_cards_dealt);
        numCardsToBeShownOnBoard = 5;


        p1_c1.setOnClickListener(cardClickListener);
        p1_c2.setOnClickListener(cardClickListener);
        b1.setOnClickListener(cardClickListener);
        b2.setOnClickListener(cardClickListener);
        b3.setOnClickListener(cardClickListener);
        b4.setOnClickListener(cardClickListener);
        b5.setOnClickListener(cardClickListener);

        // Set the card tag to default -1 (int)
        p1_c1.setTag(-1);
        p1_c2.setTag(-1);
        b1.setTag(-1);
        b2.setTag(-1);
        b3.setTag(-1);
        b4.setTag(-1);
        b5.setTag(-1);

        handler = new MyHandler(this);

        calculate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d("numOnBoard", Integer.toString(numCardsOnBoard));
                Log.d("numCardsToBeShown", Integer.toString(numCardsToBeShownOnBoard));
                if (numCardsOnBoard > numCardsToBeShownOnBoard) {
                    Toast toast = Toast.makeText(context, "Too many community cards", Toast.LENGTH_SHORT);
                    toast.show();
                }
                else {
                    // Creating a new thread allows the user to continue using the app while performing this calculation
                    // The handler sends the data from the new thread back to the main thread
                    Thread t = new Thread(new Runnable() {
                        public void run() {
                            // Convert cards arrayList<Integer> into int[]
                            int[] info = new int[]{-1,-1,-1,-1,-1,-1,-1};
                            for (int i = 0; i < cards.size(); ++i){
                                info[i] = cards.get(i);
                            }

                            Message msg = Message.obtain();
//                            long start = System.nanoTime();
                            msg.obj = getPercentages(info, numCardsToBeShownOnBoard);
//                            long stop = System.nanoTime();
//                            Log.v("Execution time", Double.toString((stop-start)/(1000000000.0)));
                            handler.sendMessage(msg);
                        }
                    });
                    t.setPriority(Thread.MAX_PRIORITY);
                    t.start();
                }
                // TODO: SHOW CALCULATING BUFFERING GRAPHIC
            }
        });

        chooseFlopTurnRiver.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, @IdRes int checkedId) {
                switch(checkedId){
                    case R.id.flop:
                        numCardsToBeShownOnBoard = 3;
                        break;
                    case R.id.turn:
                        numCardsToBeShownOnBoard = 4;
                        break;
                    case R.id.river:
                        numCardsToBeShownOnBoard = 5;
                        break;
                }
            }
        });
    }

    private static class MyHandler extends Handler{
        private final WeakReference<HandPercentages> myActivity;

        private MyHandler (HandPercentages activity){
            myActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg) {
            HandPercentages hp = myActivity.get();
            if (hp != null) {

                DecimalFormat df = new DecimalFormat("##0.0000%");
                double[] percentages = (double[]) msg.obj;
                String[] strPrct = new String[10];
                for (int i = 0; i < 10; ++i){
                    strPrct[i] = df.format(percentages[i]);
                }
                TextView handType;
                for (int i = 0; i < percentages.length; i++) {
                    switch (i) {
                        case 0:
                            handType = hp.findViewById(R.id.high_card);
                            break;
                        case 1:
                            handType = hp.findViewById(R.id.pair);
                            break;
                        case 2:
                            handType = hp.findViewById(R.id.two_pair);
                            break;
                        case 3:
                            handType = hp.findViewById(R.id.three_of_a_kind);
                            break;
                        case 4:
                            handType = hp.findViewById(R.id.straight);
                            break;
                        case 5:
                            handType = hp.findViewById(R.id.flush);
                            break;
                        case 6:
                            handType = hp.findViewById(R.id.full_house);
                            break;
                        case 7:
                            handType = hp.findViewById(R.id.four_of_a_kind);
                            break;
                        case 8:
                            handType = hp.findViewById(R.id.straight_flush);
                            break;
                        default: // (9)
                            handType = hp.findViewById(R.id.royal_flush);
                            break;
                    }
                    StringBuilder text = new StringBuilder((String) handType.getTag());
                    text.append(": ");
                    text.append(strPrct[i]);
                    handType.setText(text);
                }
                Toast toast = Toast.makeText(hp, "Calculation complete", Toast.LENGTH_SHORT);
                toast.show();
            }
        }

    }

    /****************************
     * Radio Button Manipulation*
     ****************************/

    // Make all value RBs visible
    public void replaceValueRB(){
        for (int i = 0; i < 13; i++){
            chooseValue.getChildAt(i).setVisibility(View.VISIBLE);
        }
    }

    // When we click on a suit RB, the used values of that suit should disappear
    public void removeValueRB(){
        int suitNum = Integer.parseInt((String) suit.getTag());
        String valueNum;
        // Loop through card numbers and find the values of that suit that have been used and make their buttons invisible
        for (int i = 0; i < cards.size(); ++i){
            if (cards.get(i) / 13 == suitNum){
                valueNum = Integer.toString(cards.get(i) % 13);
                chooseValue.findViewWithTag(valueNum).setVisibility(View.INVISIBLE);
            }
        }
    }

    public void replaceSuitRB(){
        for (int i = 0; i < 4; i++){
            chooseSuit.getChildAt(i).setVisibility(View.VISIBLE);
        }
    }

    public void removeSuitRB(){
        int valueNum = Integer.parseInt((String) value.getTag());
        String suitNum;
        for (int i = 0; i < cards.size(); ++i){
            if (cards.get(i) % 13 == valueNum){
                suitNum = Integer.toString(cards.get(i) / 13);
                chooseValue.findViewWithTag(suitNum).setVisibility(View.INVISIBLE);
            }
        }
    }

    /******************
     * Clicking Cards *
     ******************/
    private View.OnClickListener cardClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {

            if (v instanceof ImageView) {
                final ImageView chosenCard = (ImageView) v;
//                Log.d("Tag", chosenCard.getTag().getClass().toString()+": "+chosenCard.getTag());


                LayoutInflater inflater = (LayoutInflater) getApplicationContext().getSystemService(LAYOUT_INFLATER_SERVICE);
                ViewGroup container = (ViewGroup) inflater.inflate(R.layout.choose_card_popup, null);
                popUp = new PopupWindow(container, ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT, true);
                popUp.showAtLocation(v, Gravity.CENTER, 0, 0);

                chooseSuit = container.findViewById(R.id.choose_suit);
                chooseValue = container.findViewById(R.id.choose_value);
                chooseUnknownCard = container.findViewById(R.id.choose_unknown_card);

                chooseUnknownCard.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {

                        // Remove previous card (if it was not an unknown card)
                        int prevCardNum = (int) chosenCard.getTag();
                        if (prevCardNum != -1) {
                            cards.remove(Integer.valueOf(prevCardNum));
                            if (chosenCard.getParent() == findViewById(R.id.board)){
                                // Update number of board cards if card was on the board
                                --numCardsOnBoard;
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
                        // Change Radio Buttons
                        suit = chooseSuit.findViewById(checkedId);
                        replaceValueRB();
                        removeValueRB();
                        if (chooseValue.getCheckedRadioButtonId() != -1) { // This checks if RadioButton in value group is checked

                            // Remove previous card (if it was not an unknown card)
                            int prevCardNum = (int) chosenCard.getTag();
                            if (prevCardNum != -1) {
                                cards.remove(Integer.valueOf(prevCardNum));
                            }
                            else if (chosenCard.getParent() == findViewById(R.id.board)){
                                // Update number of board cards if card was previously unknown
                                ++numCardsOnBoard;
                            }

                            // Construct the tag (cardNum) and id
                            int cardNum = 13 * Integer.parseInt((String)suit.getTag()) + Integer.parseInt((String) value.getTag());
                            int cardID = context.getResources().getIdentifier("c"+Integer.toString(cardNum), "drawable", context.getPackageName());

                            // Set the image resource and its tag
                            chosenCard.setImageResource(cardID);
                            chosenCard.setTag(cardNum);

                            // Add cardNum to cards
                            cards.add(cardNum);

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

                            // Remove previous card (if it was not an unknown card)
                            int prevCardNum = (int) chosenCard.getTag();
                            if (prevCardNum != -1) {
                                cards.remove(Integer.valueOf(prevCardNum));
                            }
                            else if(chosenCard.getParent() == findViewById(R.id.board)){
                                // Update number of board cards if card was previously unknown
                                ++numCardsOnBoard;
                            }

                            // Construct the tag (cardNum) and id
                            int cardNum = 13 * Integer.parseInt((String) suit.getTag()) + Integer.parseInt((String) value.getTag());
                            int cardID = context.getResources().getIdentifier("c"+Integer.toString(cardNum), "drawable", context.getPackageName());

                            // Set the image resource and its tag
                            chosenCard.setImageResource(cardID);
                            chosenCard.setTag(cardNum);

                            // Add cardNum to cards
                            cards.add(cardNum);

                            popUp.dismiss();
                        }
                    }
                });

                // Close the RB pop-up if we tap somewhere other than a button
                container.setOnClickListener(new View.OnClickListener(){
                    @Override
                    public void onClick(View v){
                        popUp.dismiss();
                    }
                });

//                container.setOnTouchListener(new View.OnTouchListener() {
//                    @Override
//                    public boolean onTouch(View v, MotionEvent event) {
//                        popUp.dismiss();
//                        return false;
//                    }
//
//                });
            }
        }
    };
    public static String convertTo2SigFig(double decimal){
        if (decimal == 0.0){
            return "0.0 %";
        }
        boolean atLeastOne = false;
        if (decimal >= 1.0){
            atLeastOne = true;
        }

        String decStr = Double.toString(decimal);
        StringBuilder converted = new StringBuilder();
        int i = 0;
        // Get Front
        while(decStr.charAt(i) != '.'){
            converted.append(decStr.charAt(i));
            ++i;
        }
        converted.append('.');
        ++i;

        // Get 2 sig figs after decStr (or fewer if < 2 numbers after decimal point)
        int nonzeroCnt = 0;
        int length = decStr.length();
        while(i < length && nonzeroCnt != 2){
            converted.append(decStr.charAt(i));
            if (nonzeroCnt == 1){
                // Round last digit if next digit is greater than 5
                if (i + 1 < length && decStr.charAt(i+1) >= '5'){
                    int j = i;
                    // Propagate backwards through the decimal to round up all 9s
                    while (j >= 0 && (decStr.charAt(j) == '9' || decStr.charAt(j) == '.')){
                        if (decStr.charAt(j) == '9'){
                            converted.setCharAt(j, '0');
                        }
                        --j;
                    }
                    // If decimal consisted of all 9s (i.e. 99.9999)
                    if (j == -1){
                        converted.insert(0,'1');
                    }
                    else{
                        converted.setCharAt(j,(char)(decStr.charAt(j)+1));
                    }

                }
            }

            // Round to two decimal places unless number is below one, in this case, show 2 sig figs
            if (atLeastOne || decStr.charAt(i) != '0' || nonzeroCnt != 0){
                ++nonzeroCnt;
            }

            ++i;
        }

        // Check for scientific notation
        // Won't be smaller than 1E-99
        i = length - 4;
        boolean hitE = false;
        while (i < length){
            if (hitE){
                converted.append(decStr.charAt(i));
            }
            else if (decStr.charAt(i) == 'E'){ // THIS MAY NEED TO BE lowercase e
                converted.append('E');
                hitE = true;
            }
            ++i;
        }

        converted.append(' ');
        converted.append('%');

        return converted.toString();
    }

    // Declare C++ fcns
    public native double[] getPercentages(int[] info, int flopTurnRiver);
}
