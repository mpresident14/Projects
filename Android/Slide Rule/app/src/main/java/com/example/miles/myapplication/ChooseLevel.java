package com.example.miles.myapplication;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.provider.Contacts;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.GridLayout;
import android.widget.ImageView;
import android.widget.Toast;

import static com.example.miles.myapplication.LevelInfo.NUM_LEVELS_IMPLEMENTED;

public class ChooseLevel extends AppCompatActivity {

    Context context = this;
    GridLayout level_grid;
    ImageView level1;
    ImageView level2;
    ImageView level3;
    ImageView level4;
    ImageView level5;
    static String LEVEL_NUMBER = "level_number";

    private View.OnClickListener onClickListener = this::onLevelIconClick;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_choose_level);
        level_grid = findViewById(R.id.level_grid);
        level1 = findViewById(R.id.level1);
        level2 = findViewById(R.id.level2);
        level3 = findViewById(R.id.level3);
        level4 = findViewById(R.id.level4);
        level5 = findViewById(R.id.level5);


        // Grab level icons that were saved
        SharedPreferences sharedPref = getSharedPreferences(UIUtils.LEVEL_ICON, Context.MODE_PRIVATE);
        String currentLevel;
        int levelsCompleted = sharedPref.getInt(UIUtils.LEVELS_COMPLETED, 0);
        ImageView levelIconImageView;

        // Set number of stars and activate clickability for completed levels
        for (int i = 0; i < levelsCompleted+1; i++) {
            currentLevel = Integer.toString(i+1);
            // Grab ImageView at specified index
            levelIconImageView = (ImageView) level_grid.getChildAt(i);
            // Get the correct stars icon that we stored
            int numStars = sharedPref.getInt(currentLevel+UIUtils.STARS, 0);
            int drawableId = UIUtils.intToDrawableIdNumStars(numStars);
            // Update attributes for completed levels
            levelIconImageView.setImageResource(drawableId);
            ((View)levelIconImageView).setAlpha(1);
            levelIconImageView.setClickable(true);
        }

        // Set up click listeners for completed levels
        View v;
        for (int level = 1; level < levelsCompleted + 2; level++) {
            if (level <= NUM_LEVELS_IMPLEMENTED) {
                v = level_grid.getChildAt(level - 1);
                v.setOnClickListener(onClickListener);
            }
            else if (level == NUM_LEVELS_IMPLEMENTED + 1) {
                v = level_grid.getChildAt(level - 1);
                v.setOnClickListener((view)->
                        Toast.makeText(context, "Next level not implemented yet.", Toast.LENGTH_SHORT).show());
            }
            else {
                break;
            }
        }
    }

    private void onLevelIconClick(View view) {
        Intent intent = new Intent(ChooseLevel.this, GenericLevel.class);
        intent.putExtra(LEVEL_NUMBER, Integer.parseInt((String) view.getTag()));
        startActivity(intent);
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Intent intent = new Intent(ChooseLevel.this, TitleScreen.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
        intent.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
        startActivity(intent);
    }
}


