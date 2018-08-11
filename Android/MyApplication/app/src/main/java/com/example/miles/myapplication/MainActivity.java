package com.example.miles.myapplication;

import android.icu.text.DecimalFormat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    EditText inputAmount;
    TextView tipPercentage;
    TextView numPeople;
    TextView tipAmount;
    TextView perPerson;
    SeekBar tipSlider;
    SeekBar peopleSlider;
    Button button;
    double subtotal;
    double tip;
    double roundedTip;
    double amountPerPerson;
    double roundedAmountPerPerson;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        inputAmount = (EditText) findViewById(R.id.amountInput);
        tipAmount = (TextView) findViewById(R.id.tipAmountText);
        button = (Button) findViewById(R.id.button);
        tipSlider = (SeekBar) findViewById(R.id.tipSlider);
        tipPercentage = (TextView) findViewById(R.id.tipPercentText);
        peopleSlider = (SeekBar) findViewById(R.id.peopleSlider);
        numPeople = (TextView) findViewById(R.id.numPeopleText);
        perPerson = (TextView) findViewById(R.id.amountPerPersonText);

        button.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                calculate();
            }
        });

        tipSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                tipPercentage.setText("Tip Percentage: " + Integer.toString(progress) + "%");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        peopleSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int value = progress + 1;
                numPeople.setText("Number of People:" + Integer.toString(value));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    public void calculate(){
        subtotal = Double.parseDouble(inputAmount.getText().toString());
        tip = subtotal * tipSlider.getProgress()/100;
        roundedTip = Math.round(tip*100.0)/100.0;
        amountPerPerson = (subtotal + roundedTip)/(peopleSlider.getProgress()+1);
        roundedAmountPerPerson = Math.round(amountPerPerson*100.0)/100.0;

        tipAmount.setText("Tip Amount: $ " + Double.toString(roundedTip));
        perPerson.setText("Amount Per Person: $" + Double.toString(roundedAmountPerPerson));

    }
}

