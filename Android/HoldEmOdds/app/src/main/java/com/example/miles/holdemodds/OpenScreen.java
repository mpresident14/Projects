package com.example.miles.holdemodds;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class OpenScreen extends AppCompatActivity {



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_open_screen);

        Button winButton = (Button) findViewById(R.id.winPercentages);
        Button handButton = (Button) findViewById(R.id.handPercentages);

        winButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(OpenScreen.this, WinPercentages.class);
                startActivity(intent);
            }
        });

        handButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(OpenScreen.this, HandPercentages.class);
                startActivity(intent);
            }
        });
    }


}
