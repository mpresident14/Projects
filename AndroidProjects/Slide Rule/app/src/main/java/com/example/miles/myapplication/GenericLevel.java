package com.example.miles.myapplication;

import android.animation.Animator;
import android.content.Context;
import android.content.Intent;
import android.databinding.DataBindingUtil;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewPropertyAnimator;
import android.widget.GridLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.example.miles.myapplication.databinding.ActivityGenericLevelBinding;

import java.util.Locale;

public class GenericLevel extends AppCompatActivity {

    ActivityGenericLevelBinding binder;
    private static int levelNumber;
    private LevelInfo levelInfo;

    // TODO: Implement onPerformClick for accessibility.
    // See https://stackoverflow.com/questions/47107105/android-button-has-setontouchlistener-called-on-it-but-does-not-override-perform
    private View.OnTouchListener onTouchListener = this::swipe;

    private GridLayout gridLayout;
    // ---------------------------------

    Context context = this;
    // TODO: Create a separate boolean for this
    boolean disableTouch = false; // Also represents a completed board b/c we disable touch when the board is correct
    boolean boardIsCorrect = false;
    View movingCurrently = null;

    float x1,x2;
    float y1,y2;

    // THESE ARE THE ONLY VARIABLES THAT NEED TO BE CHANGED FOR EACH LEVEL
    Board board;
    Context thisContext = GenericLevel.this;

    int moves2Stars;
    int moves3Stars;

    int numCellsForTileToTraverseOnSwipe;
    int numMoves = 0;

    LinearLayout rowSolutionDisplay;
    LinearLayout columnSolutionDisplay;
    TextView moveDisplay;
    ImageView restartButton;
    ImageView backButton;

    ViewPropertyAnimator moveAnimation;

    static float translateDistance;
    static final int translateDuration = 100;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binder = DataBindingUtil.setContentView(this, R.layout.activity_generic_level);
        levelNumber = getIntent().getIntExtra(ChooseLevel.LEVEL_NUMBER, -1);
        levelInfo = new LevelInfo(levelNumber);

        rowSolutionDisplay = binder.rowSolutions;
        columnSolutionDisplay = binder.columnSolutions;
        gridLayout = binder.grid;
        restartButton = binder.restart;
        backButton = binder.back;
        moveDisplay = binder.numMoves;

        board = new Board(
                levelInfo.grid,
                levelInfo.rowSolutions,
                levelInfo.colSolutions,
                levelInfo.nailedRow,
                levelInfo.nailedCol);

        translateDistance = UIUtils.convertDpToPixels(60, context); // 60dp is the width of a cell
        numMoves = 0;

        setCellTextViews();
        setNailedTextViews();
        setSolutionTextViews();
        setLevelNumberAndMovesForStars();

        restartButton.setOnClickListener((view) -> {
            Intent intent = new Intent(thisContext, GenericLevel.class);
            intent.putExtra(ChooseLevel.LEVEL_NUMBER, levelNumber);
            startActivity(intent);
        });

        backButton.setOnClickListener((view)-> {
            Intent intent = new Intent(thisContext, ChooseLevel.class);
            startActivity(intent);
        });
    }

    private void setCellTextViews() {
        int childIndex = 0;
        int numRows = levelInfo.grid.length;
        int numColumns = levelInfo.grid[0].length;
        for (int row = 0; row < numRows; row++) {
            for (int col = 0; col < numColumns; col++) {
                int currentCellChar = levelInfo.grid[row][col];
                if (currentCellChar != Board.EMPTY) {

                    TextView cell = (TextView) gridLayout.getChildAt(childIndex);
                    cell.setBackground(getResources().getDrawable(R.drawable.movable_tile_white));
                    cell.setOnTouchListener(onTouchListener);
                    if (Board.isOperatorAsciiValue(currentCellChar)) {
                        cell.setText(Character.toString((char)currentCellChar));
                    } else {
                        cell.setText(String.valueOf(currentCellChar));
                    }
                }

                childIndex++;
            }
        }
    }

    private void setNailedTextViews() {
        int numsRows = levelInfo.nailedCol.length;
        for (int row = 0; row < numsRows; row++) {
            int numInCell = levelInfo.nailedCol[row];
            if (numInCell != Board.EMPTY) {
                TextView cell = (TextView) binder.nailedCol.getChildAt(row);
                cell.setBackground(getResources().getDrawable(R.drawable.nailed_tile_white));
                cell.setText(String.valueOf(numInCell));
            }
        }

        int numsCols = levelInfo.nailedRow.length;
        for (int col = 0; col < numsCols; col++) {
            int numInCell = levelInfo.nailedRow[col];
            if (numInCell != Board.EMPTY) {
                TextView cell = (TextView) binder.nailedRow.getChildAt(col);
                cell.setBackground(getResources().getDrawable(R.drawable.nailed_tile_white));
                cell.setText(String.valueOf(numInCell));
            }
        }
    }

    private void setSolutionTextViews() {
        int numsRows = levelInfo.rowSolutions.length;
        for (int row = 0; row < numsRows; row++) {
            int numInCell = levelInfo.rowSolutions[row];
            if (numInCell != Board.EMPTY) {
                TextView cell = (TextView) binder.rowSolutions.getChildAt(row);
                cell.setBackground(getResources().getDrawable(R.drawable.solution_wrong_tile));
                cell.setText(String.valueOf(numInCell));
            }
        }

        int numsCols = levelInfo.colSolutions.length;
        for (int col = 0; col < numsCols; col++) {
            int numInCell = levelInfo.colSolutions[col];
            if (numInCell != Board.EMPTY) {
                TextView cell = (TextView) binder.columnSolutions.getChildAt(col);
                cell.setBackground(getResources().getDrawable(R.drawable.solution_wrong_tile));
                cell.setText(String.valueOf(numInCell));
            }
        }
    }

    private void setLevelNumberAndMovesForStars() {
        moves2Stars = levelInfo.moves2Stars;
        moves3Stars = levelInfo.moves3Stars;
        binder.twoStarsTv.setText(String.valueOf(moves2Stars));
        binder.threeStarsTv.setText(String.valueOf(moves3Stars));

        binder.levelNumberTv.setText(String.valueOf(levelNumber));
    }

    public boolean swipe(View v, MotionEvent event){
        // Don't allow a swipe if we are currently moving a tile or if the level has been completed
        if (disableTouch){
            return false;
        }
        // Don't allow a swipe if moveAnimator is currently moving this view
        if (movingCurrently != null && movingCurrently.equals(v)){
            return false;
        }

        switch (event.getActionMasked()){
            case MotionEvent.ACTION_DOWN:
                x1 = event.getX();
                y1 = event.getY();
                return true;
            case MotionEvent.ACTION_UP:
                disableTouch = true; // Disable touch while we update the board encoding
                x2 = event.getX();
                y2 = event.getY();

                if (x1 < x2 && x2 - x1 > Math.abs(y2 - y1)){
                    numCellsForTileToTraverseOnSwipe = board.swipeRight(UIUtils.getRow(v,translateDistance), UIUtils.getCol(v, translateDistance));
                    disableTouch = false;
                    if (numCellsForTileToTraverseOnSwipe != 0){
                        moveRight(v, numCellsForTileToTraverseOnSwipe);
                    }
                }
                else if (y1 < y2 && y2 - y1 > Math.abs(x2 - x1)){
                    numCellsForTileToTraverseOnSwipe = board.swipeDown(UIUtils.getRow(v,translateDistance), UIUtils.getCol(v, translateDistance));
                    disableTouch = false;
                    if (numCellsForTileToTraverseOnSwipe != 0){
                        moveDown(v, numCellsForTileToTraverseOnSwipe);
                    }
                }
                else if (x1 > x2 && x1 - x2 > Math.abs(y2 - y1)){
                    numCellsForTileToTraverseOnSwipe = board.swipeLeft(UIUtils.getRow(v,translateDistance), UIUtils.getCol(v, translateDistance));
                    disableTouch = false;
                    if (numCellsForTileToTraverseOnSwipe != 0){
                        moveLeft(v, numCellsForTileToTraverseOnSwipe);
                    }
                }
                else if (y1 > y2 && y1 - y2 > Math.abs(x2 - x1)){
                    numCellsForTileToTraverseOnSwipe = board.swipeUp(UIUtils.getRow(v,translateDistance), UIUtils.getCol(v, translateDistance));
                    disableTouch = false;
                    if (numCellsForTileToTraverseOnSwipe != 0){
                        moveUp(v, numCellsForTileToTraverseOnSwipe);
                    }
                }

                // Check the board to see if it is correct (if board is correct, we also disable touch)
                disableTouch = boardIsCorrect = board.checkEquations(rowSolutionDisplay, columnSolutionDisplay);

                // Check the board and change colors when the animation finishes
                moveAnimation.setListener(new Animator.AnimatorListener() {
                    @Override
                    public void onAnimationStart(Animator animation) {
                        // Nothing to do
                    }

                    @Override
                    public void onAnimationEnd(Animator animation) {
                        // Update the number of moves
                        numMoves++;
                        moveDisplay.setText(String.format(Locale.getDefault(),"%d",numMoves));

                        // Initiate CompletionPopUp if level is correct
                        if (boardIsCorrect){
                            int numStars = UIUtils.getNumStars(numMoves, moves3Stars, moves2Stars);
                            UIUtils.changeStarColor(context, levelNumber, numStars);
                            Intent intent = new Intent(thisContext, CompletionPopUp.class);
                            intent.putExtra(CompletionPopUp.NUM_STARS, numStars);
                            intent.putExtra(CompletionPopUp.CURRENT_LEVEL, levelNumber);
                            startActivity(intent);
                        }
                        movingCurrently = null;
                    }

                    @Override
                    public void onAnimationCancel(Animator animation) {
                        // Nothing to do
                    }

                    @Override
                    public void onAnimationRepeat(Animator animation) {
                        // Nothing to do
                    }
                });

                return true;
        }
        return false;
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Intent intent = new Intent(thisContext, ChooseLevel.class);
        startActivity(intent);
    }

    public void moveRight(View v, int shifts){
        movingCurrently = v;
        moveAnimation = v.animate().translationXBy(shifts*translateDistance).setDuration(shifts*translateDuration);
    }
    public void moveLeft(View v, int shifts){
        movingCurrently = v;
        moveAnimation = v.animate().translationXBy(-shifts*translateDistance).setDuration(shifts*translateDuration);
    }
    public void moveUp(View v, int shifts){
        movingCurrently = v;
        moveAnimation = v.animate().translationYBy(-shifts*translateDistance).setDuration(shifts*translateDuration);
    }
    public void moveDown(View v, int shifts){
        movingCurrently = v;
        moveAnimation = v.animate().translationYBy(shifts*translateDistance).setDuration(shifts*translateDuration);
    }
}

