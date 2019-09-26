package com.mcprez.calculator;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import bsh.EvalError;
import bsh.Interpreter;

public class MainActivity extends AppCompatActivity {

    boolean decimalBeforeDivision = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        final TextView display = (TextView) findViewById(R.id.textView7);
        Button oneBtn = (Button)findViewById(R.id.oneBtn);
        Button twoBtn = (Button)findViewById(R.id.twoBtn);
        Button threeBtn = (Button)findViewById(R.id.threeBtn);
        Button fourBtn = (Button)findViewById(R.id.fourBtn);
        Button fiveBtn = (Button)findViewById(R.id.fiveBtn);
        Button sixBtn = (Button)findViewById(R.id.sixBtn);
        Button sevenBtn = (Button)findViewById(R.id.sevenBtn);
        Button eightBtn = (Button)findViewById(R.id.eightBtn);
        Button nineBtn = (Button)findViewById(R.id.nineBtn);
        Button zeroBtn = (Button)findViewById(R.id.zeroBtn);
        Button clearBtn = (Button)findViewById(R.id.clearBtn);
        Button decimalBtn = (Button)findViewById(R.id.decimalBtn);
        Button backBtn = (Button)findViewById(R.id.backBtn);
        Button leftParenthesisBtn = (Button)findViewById(R.id.leftParenthesisBtn);
        Button rightParenthesisBtn = (Button)findViewById(R.id.rightParenthesisBtn);
        Button plusBtn = (Button)findViewById(R.id.plusBtn);
        Button minusBtn = (Button)findViewById(R.id.minusBtn);
        Button multiplyBtn = (Button)findViewById(R.id.multiplyBtn);
        Button divideBtn = (Button)findViewById(R.id.divideBtn);
        Button equalsBtn = (Button)findViewById(R.id.equalsBtn);



        // Numbers
        oneBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("1");
                }
                else{
                    display.setText(display.getText() + "1");
                }
            }
        });

        twoBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("2");
                }
                else{
                    display.setText(display.getText() + "2");
                }
            }
        });

        threeBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("3");
                }
                else{
                    display.setText(display.getText() + "3");
                }
            }
        });

        fourBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("4");
                }
                else{
                    display.setText(display.getText() + "4");
                }
            }
        });

        fiveBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("5");
                }
                else{
                    display.setText(display.getText() + "5");
                }
            }
        });

        sixBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("6");
                }
                else{
                    display.setText(display.getText() + "6");
                }
            }
        });

        sevenBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("7");
                }
                else{
                    display.setText(display.getText() + "7");
                }
            }
        });

        eightBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("8");
                }
                else{
                    display.setText(display.getText() + "8");
                }
            }
        });

        nineBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("9");
                }
                else{
                    display.setText(display.getText() + "9");
                }
            }
        });

        zeroBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("0") || display.getText().toString().equals("Error")){
                    display.setText("0");
                }
                else{
                    display.setText(display.getText() + "0");
                }
            }
        });

        //Operators
        plusBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("") || display.getText().toString().equals("Error")){
                    display.setText("0+");
                }
                else{
                    display.setText(display.getText() + "+");
                }
            }
        });

        minusBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String text = display.getText().toString();
                String last = getLast(text);
                if (last.equals("-")){
                    display.setText(display.getText() + "(-");
                }
                else if(display.getText().toString().equals("") || display.getText().toString().equals("Error")){
                    display.setText("0-");
                }
                else{
                    display.setText(display.getText() + "-");
                }
            }
        });

        multiplyBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("") || display.getText().toString().equals("Error")){
                    display.setText("0*");
                }
                else{
                    display.setText(display.getText() + "*");
                }
            }
        });

        divideBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("") || display.getText().toString().equals("Error")){
                    display.setText("0/");
                }
                else if(!decimalBeforeDivision){
                    display.setText(display.getText() + ".0/");
                    decimalBeforeDivision = false;
                }
                else{
                    display.setText(display.getText() + "/");
                }
            }


        });

        // Other Functions
        decimalBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                display.setText(display.getText() + ".");
                decimalBeforeDivision = true;
            }
        });

        leftParenthesisBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("") || display.getText().toString().equals("Error")){
                    display.setText("(");
                }
                else{
                    String text = display.getText().toString();
                    String last = getLast(text);
                    switch (last){
                        case "+" :
                        case "-":
                        case "*":
                        case "/":
                            display.setText(display.getText() + "(");
                            break;
                        default:
                            display.setText(display.getText() + "*(");
                    }
                }


            }
        });

        rightParenthesisBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (display.getText().toString().equals("") || display.getText().toString().equals("Error")){
                    display.setText(")");
                }
                else{
                    display.setText(display.getText() + ")");
                }
            }
        });

        backBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String equation = display.getText().toString();
                if (equation.equals("") || equation.equals("Error")){
                    display.setText("");
                }
                else{
                    String last = getLast(equation);
                    String text = removeLast(equation);
                    display.setText(text);
                    if (last.equals(".")){
                        decimalBeforeDivision = false;
                    }
                }
            }
        });



        clearBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                display.setText("");
            }
        });

        equalsBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Object result = calculate(display.getText().toString());
                String text = result.toString();
                display.setText(text);
                if (text.contains(".")){
                    decimalBeforeDivision = true;
                }
                else {
                    decimalBeforeDivision = false;
                }
            }
        });


    }

    public Object calculate(String equation){
        Object result = "Error";
        Interpreter interpreter = new Interpreter();
        try{
            result = interpreter.eval(equation);

            return result;

        }
        catch (EvalError e){
            System.out.println("Error");
            decimalBeforeDivision = false;
        }
        return result;
    }

    public String getLast(String str){
        char lastChar = str.charAt(str.length()-1);
        return Character.toString(lastChar);
    }

    public String removeLast(String str){
        int length = str.length();
        if (str.equals("")){
            return "";
        }
        else{
            return str.substring(0, length-1);
        }
    }




}
