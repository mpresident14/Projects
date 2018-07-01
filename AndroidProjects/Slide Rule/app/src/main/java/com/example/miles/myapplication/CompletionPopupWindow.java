package com.example.miles.myapplication;

import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.widget.ImageView;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.Toast;
import java.util.Locale;

import static com.example.miles.myapplication.LevelInfo.NUM_LEVELS_IMPLEMENTED;

final class CompletionPopupWindow {

    private PopupWindow popupWindow;

    CompletionPopupWindow(View contentView,
                          Context context,
                          int width,
                          int height,
                          int numStars,
                          int numMoves,
                          int levelNumber) {
        popupWindow = new PopupWindow(contentView, width, height);
//        popupWindow.setAnimationStyle(R.style.PopupAnimation);

        ImageView star2 = contentView.findViewById(R.id.star2);
        ImageView star3 = contentView.findViewById(R.id.star3);
        TextView numMovesTextView = contentView.findViewById(R.id.popup_num_moves_number_tv);
        ImageView restart = contentView.findViewById(R.id.restart);
        ImageView back = contentView.findViewById(R.id.back);
        ImageView next = contentView.findViewById(R.id.next);

        // Change the correct number of stars to gold
        if (numStars >= 2){
            star2.setImageResource(R.drawable.gold_star);
            if (numStars == 3){
                star3.setImageResource(R.drawable.gold_star);
            }
        }

        // Set display to show # of moves it took to complete the level
        numMovesTextView.setText(String.format(Locale.getDefault(), "%d", numMoves));

        restart.setOnClickListener(view -> {
            Intent intent = new Intent(context, GenericLevel.class);
            intent.putExtra(ChooseLevel.LEVEL_NUMBER, levelNumber);
            context.startActivity(intent);
        });

        back.setOnClickListener(view -> {
            Intent intent = new Intent(context, ChooseLevel.class);
            context.startActivity(intent);
        });

        next.setOnClickListener(view -> {
            int nextLevel = levelNumber + 1;
            Intent intent = new Intent(context, GenericLevel.class);
            intent.putExtra(ChooseLevel.LEVEL_NUMBER, nextLevel);
            if (nextLevel <= NUM_LEVELS_IMPLEMENTED) {
                context.startActivity(intent);
            }
            else {
                Toast.makeText(context, "Next level not implemented yet.", Toast.LENGTH_SHORT).show();
            }
        });
    }

    PopupWindow getPopupWindow() {
        return popupWindow;
    }
}
