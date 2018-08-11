package com.example.miles.myapplication;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class TitleScreen extends AppCompatActivity {

    Button play;
    Button settings;
    Button tutorial;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_title_screen);

        play = (Button) findViewById(R.id.play);
        settings = (Button) findViewById(R.id.settings);
        tutorial = (Button) findViewById(R.id.tutorial);

        play.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(TitleScreen.this, ChooseLevel.class);
                startActivity(intent);
            }
        });
        // TO DO: set other onClickListeners
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        finish();
    }
}
