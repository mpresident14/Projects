package com.example.miles.myapplication;

import android.content.Context;
import android.content.Intent;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Locale;

import static com.example.miles.myapplication.LevelInfo.NUM_LEVELS_IMPLEMENTED;

public class CompletionPopUp extends Fragment {

    Context context;

    private ImageView star2;
    private ImageView star3;
    private TextView numMovesTextView;
    private ImageView restart;
    private ImageView back;
    private ImageView next;
    private View.OnClickListener outOfPopUpClickListener;

    final static String NUM_STARS = "num_stars";
    final static String CURRENT_LEVEL = "current_level";
    final static String NUM_MOVES = "num_moves";

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        this.context = context;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {

        // Set popup window size based on a percentage of phone screen size
//        DisplayMetrics dm = new DisplayMetrics();
//        getActivity().getWindowManager().getDefaultDisplay().getMetrics(dm);
//        int width = dm.widthPixels;
//        int height = dm.heightPixels;
//        getActivity().getWindow().setLayout((int) (width * 0.8), (int) (height * 0.5));

        return inflater.inflate(R.layout.fragment_completion_pop_up, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        star2 = view.findViewById(R.id.star2);
        star3 = view.findViewById(R.id.star3);
        numMovesTextView = view.findViewById(R.id.popup_num_moves_number_tv);
        restart = view.findViewById(R.id.restart);
        back = view.findViewById(R.id.back);
        next = view.findViewById(R.id.next);

        // Change the correct number of stars to gold
        Bundle completionInfoBundle = getArguments();
        int numStars = completionInfoBundle.getInt(NUM_STARS,1);
        if (numStars >= 2){
            star2.setImageResource(R.drawable.gold_star);
            if (numStars == 3){
                star3.setImageResource(R.drawable.gold_star);
            }
        }

        int numMoves = completionInfoBundle.getInt(NUM_MOVES, -1);
        numMovesTextView.setText(String.format(Locale.getDefault(), "%d", numMoves));

        restart.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                int currentLevel = completionInfoBundle.getInt(CURRENT_LEVEL, -1);
                Intent intent = new Intent(context, GenericLevel.class);
                intent.putExtra(ChooseLevel.LEVEL_NUMBER, currentLevel);
                startActivity(intent);
            }
        });

        back.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                Intent intent = new Intent(context, ChooseLevel.class);
                startActivity(intent);
            }
        });

        next.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                int nextLevel = completionInfoBundle.getInt(CURRENT_LEVEL, -1) + 1;
                Intent intent = new Intent(context, GenericLevel.class);
                intent.putExtra(ChooseLevel.LEVEL_NUMBER, nextLevel);
                if (nextLevel <= NUM_LEVELS_IMPLEMENTED) {
                    startActivity(intent);
                }
                else {
                    Toast.makeText(context, "Next level not implemented yet.", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

//    @Override
//    public void onBackPressed(){
//        super.onBackPressed();
//        Intent intent = new Intent(CompletionPopUp.this, ChooseLevel.class);
//        startActivity(intent);
//    }
}
