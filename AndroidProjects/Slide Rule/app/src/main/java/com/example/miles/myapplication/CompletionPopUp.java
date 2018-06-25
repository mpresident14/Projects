package com.example.miles.myapplication;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.provider.ContactsContract;
import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import static com.example.miles.myapplication.LevelInfo.NUM_LEVELS_IMPLEMENTED;

public class CompletionPopUp extends AppCompatActivity {

    Context context = this;

    ImageView star2;
    ImageView star3;
    ImageView restart;
    ImageView back;
    ImageView next;

    final static String NUM_STARS = "num_stars";
    final static String CURRENT_LEVEL = "current_level";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_completion_pop_up);

        // Set popup window size based on a percentage of phone screen size
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);
        int width = dm.widthPixels;
        int height = dm.heightPixels;
        getWindow().setLayout((int) (width * 0.8), (int) (height * 0.5));

        star2 = findViewById(R.id.star2);
        star3 = findViewById(R.id.star3);
        restart = findViewById(R.id.restart);
        back = findViewById(R.id.back);
        next = findViewById(R.id.next);

        // Change the correct number of stars to gold
        int numStars = getIntent().getIntExtra(NUM_STARS,1);
        if (numStars >= 2){
            star2.setImageResource(R.drawable.gold_star);
            if (numStars == 3){
                star3.setImageResource(R.drawable.gold_star);
            }
        }

        restart.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                int currentLevel = getIntent().getIntExtra(CURRENT_LEVEL, -1);
                Intent intent = new Intent(CompletionPopUp.this, GenericLevel.class);
                intent.putExtra(ChooseLevel.LEVEL_NUMBER, currentLevel);
                startActivity(intent);
            }
        });

        back.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                Intent intent = new Intent(CompletionPopUp.this, ChooseLevel.class);
                startActivity(intent);
            }
        });

        next.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                int nextLevel = getIntent().getIntExtra(CURRENT_LEVEL, -1) + 1;
                Intent intent = new Intent(CompletionPopUp.this, GenericLevel.class);
                intent.putExtra(ChooseLevel.LEVEL_NUMBER, nextLevel);
                if (nextLevel < NUM_LEVELS_IMPLEMENTED) {
                    startActivity(intent);
                }
                else {
                    Toast.makeText(context, "Next level not implemented yet.", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }
    @Override
    public void onBackPressed(){
        super.onBackPressed();
        Intent intent = new Intent(CompletionPopUp.this, ChooseLevel.class);
        startActivity(intent);
    }
}
