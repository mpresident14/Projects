package com.example.miles.myapplication;

import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;
import android.util.TypedValue;
import android.view.View;
import android.widget.LinearLayout;

public class UIUtils {

    static String STARS = "_stars";
    static String LEVEL_ICON = "level_icon";
    static String LEVELS_COMPLETED = "levels_completed";

    private UIUtils(){}

    static void changeSolutionBackgroundColor(LinearLayout solutionDisplay, int childIndex, boolean isCorrect) {
        if (isCorrect) {
            solutionDisplay.getChildAt(childIndex).setBackgroundResource(R.drawable.solution_correct_tile);
        }
        else {
            solutionDisplay.getChildAt(childIndex).setBackgroundResource(R.drawable.solution_wrong_tile);
        }
    }

    static int getRow(View v, float translateDistance){
        return Math.round(v.getY()/translateDistance);
    }
    static int getCol(View v, float translateDistance){
        return Math.round(v.getX()/translateDistance);
    }
    static int convertDpToPixels(float dp, Context context) {
        return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dp, context.getResources().getDisplayMetrics());
    }
    static int getNumStars(int numMoves, int moves3Stars, int moves2Stars){
        if (numMoves <= moves3Stars){
            return 3;
        }
        if (numMoves <= moves2Stars){
            return 2;
        }
        return 1;
    }

    /** Update the SharedPreferences object that we pass to the onCreate method in ChooseLevel
     * level number -> level icon drawable id showing number of stars for that level
     * levels completed  -> number of levels completed     *
     * */

    static void changeStarColor(Context context, int levelNumber, int numStars){

        SharedPreferences sharedPref = context.getSharedPreferences(LEVEL_ICON, Context.MODE_PRIVATE);
        String stringLevelNumber = Integer.toString(levelNumber);

        // Get current number of stars (we don't want update to fewer stars)
        int currentStars = sharedPref.getInt(stringLevelNumber+STARS, 0);

        // Update if the most recent number of stars received is greater than the previous amount
        if (numStars > currentStars){
            SharedPreferences.Editor editor = sharedPref.edit();
            editor.putInt(stringLevelNumber+STARS, numStars);

            // Possibly update the number of levels completed
            if (sharedPref.getInt(LEVELS_COMPLETED, 0) < levelNumber){
                editor.putInt(LEVELS_COMPLETED, levelNumber);
            }

            editor.apply();
        }
    }

    static int intToDrawableIdNumStars(int n){
        switch (n){
            case 1:
                return R.drawable.one_star;
            case 2:
                return R.drawable.two_stars;
            case 3:
                return R.drawable.three_stars;
            default:
                return R.drawable.no_stars;
        }
    }

    static int drawableIdNumStarsToInt(int id){
        switch (id){
            case R.drawable.one_star:
                return 1;
            case R.drawable.two_stars:
                return 2;
            case R.drawable.three_stars:
                return 3;
            default:
                return 0;
        }
    }
}
