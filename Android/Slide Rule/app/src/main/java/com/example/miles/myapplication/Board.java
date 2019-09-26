package com.example.miles.myapplication;

import android.graphics.Color;
import android.util.Log;
import android.widget.GridLayout;
import android.widget.LinearLayout;
import android.widget.Toast;

/**
 * Created by Miles on 12/30/2017.
 */

public class Board {
    // Rows / Cols without solutions are designated as Integer.MAX_VALUE
    private int[] rowSolutions;
    private int[] colSolutions;
    private int[] nailedRow;
    private int[] nailedCol;
    private int[][] grid;

    static int EMPTY = Integer.MAX_VALUE; // Representation of a invalid equation or an empty space in row/col solution array
    static int INVALID = Integer.MAX_VALUE; // Not a valid equation to calculate a value for

    public Board(int[][] grid, int[] rowSolutions, int[] colSolutions, int[] nailedRow, int[] nailedCol){
        this.grid = grid;
        this.rowSolutions = rowSolutions;
        this.colSolutions = colSolutions;
        this.nailedRow = nailedRow;
        this.nailedCol = nailedCol;
    }

    // Return # of positions right to shift, 0 if it is an invalid swipe
    // Don't have to worry about swiping on a null position b/c no onTouchListener
    public int swipeRight(int row, int col){
        int newCol = col;
        int numCellsForTileToTraverseOnSwipe = 0;
        while(newCol < this.grid[0].length - 1 && this.grid[row][newCol+1] == EMPTY){
            newCol++;
            numCellsForTileToTraverseOnSwipe++;
        }

        if (numCellsForTileToTraverseOnSwipe != 0){
            this.grid[row][newCol] = this.grid[row][col];
            this.grid[row][col] = EMPTY;
        }

        return numCellsForTileToTraverseOnSwipe;
    }

    public int swipeLeft(int row, int col){
        int newCol = col;
        int numCellsForTileToTraverseOnSwipe = 0;
        while(newCol > 0 && this.grid[row][newCol-1] == EMPTY){
            newCol--;
            numCellsForTileToTraverseOnSwipe++;
        }

        if (numCellsForTileToTraverseOnSwipe != 0){
            this.grid[row][newCol] = this.grid[row][col];
            this.grid[row][col] = EMPTY;
        }

        return numCellsForTileToTraverseOnSwipe;
    }

    public int swipeDown(int row, int col){
        int newRow = row;
        int numCellsForTileToTraverseOnSwipe = 0;
        while(newRow < this.grid.length - 1 && this.grid[newRow+1][col] == EMPTY){
            newRow++;
            numCellsForTileToTraverseOnSwipe++;
        }

        if (numCellsForTileToTraverseOnSwipe != 0){
            this.grid[newRow][col] = this.grid[row][col];
            this.grid[row][col] = EMPTY;
        }

        return numCellsForTileToTraverseOnSwipe;
    }

    public int swipeUp(int row, int col){
        int newRow = row;
        int numCellsForTileToTraverseOnSwipe = 0;
        while(newRow > 0 && this.grid[newRow-1][col] == EMPTY){
            newRow--;
            numCellsForTileToTraverseOnSwipe++;
        }

        if (numCellsForTileToTraverseOnSwipe != 0){
            this.grid[newRow][col] = this.grid[row][col];
            this.grid[row][col] = EMPTY;
        }

        return numCellsForTileToTraverseOnSwipe;
    }

    private int getRowTotal(int row){

        double result;
        boolean operatorNext;
        int currentOperator;

        if (this.nailedCol[row] != EMPTY){
            result = this.nailedCol[row];
            operatorNext = true;
            currentOperator = ' ';
        }
        else {
            result = 0;
            operatorNext = false;
            currentOperator = '+';
        }

        int i = 0;
        while(i != grid[row].length){
            if (grid[row][i] == EMPTY){
                i++;
                continue;
            }
            else if (operatorNext){
                if (!isOperatorAsciiValue(grid[row][i])){
                    return INVALID;
                }
                currentOperator = grid[row][i];
                operatorNext = false;
            }
            else{
                if (isOperatorAsciiValue(grid[row][i])){
                    return INVALID;
                }
                switch (currentOperator){
                    case '+':
                        result += (int)grid[row][i];
                        break;
                    case '-':
                        result -= (int)grid[row][i];
                        break;
                    case '×':
                        result *= (int)grid[row][i];
                        break;
                    case '÷':
                        result /= (int)grid[row][i];
                        break;
                }
                operatorNext = true;
            }
            i++;
        }
        // Only return if last element was a number
        if (operatorNext && Math.floor(result) == result){
            return (int)result;
        }
        return EMPTY;
    }

    private int getColumnTotal(int col){

        double result;
        boolean operatorNext;
        int currentOperator;

        if (this.nailedRow[col] != EMPTY){
            result = this.nailedRow[col];
            operatorNext = true;
            currentOperator = ' ';
        }
        else {
            result = 0;
            operatorNext = false;
            currentOperator = '+';
        }

        int i = 0;
        while(i != grid.length){
            if (grid[i][col] == EMPTY){
                i++;
                continue;
            }
            else if (operatorNext){
                if (!isOperatorAsciiValue(grid[i][col])){
                    return INVALID;
                }
                currentOperator = grid[i][col];
                operatorNext = false;
            }
            else{
                if (isOperatorAsciiValue(grid[i][col])){
                    return INVALID;
                }
                switch (currentOperator){
                    case '+':
                        result += grid[i][col];
                        break;
                    case '-':
                        result -= grid[i][col];
                        break;
                    case '×':
                        result *= grid[i][col];
                        break;
                    case '÷':
                        result /= grid[i][col];
                        break;
                }
                operatorNext = true;
            }
            i++;
        }
        // Only return if last element was a number && make sure result is a whole number
        if (operatorNext && Math.floor(result) == result){
            return (int)result;
        }
        return INVALID;
    }

    public boolean checkEquations(LinearLayout rowSolutionDisplay, LinearLayout colSolutionDisplay){
        boolean gridIsCorrect = true;
        int numRows = this.grid.length;
        for (int row = 0; row < numRows; ++row){
            if (this.rowSolutions[row] != EMPTY){
                boolean rowIsCorrect = this.rowSolutions[row] == this.getRowTotal(row);
                UIUtils.changeSolutionBackgroundColor(rowSolutionDisplay, row, rowIsCorrect);
                if (gridIsCorrect) {
                    gridIsCorrect = rowIsCorrect;
                }
            }
        }

        int numCols = this.grid[0].length;
        for (int col = 0; col < numCols; ++col){
            if (this.colSolutions[col] != EMPTY){
                boolean colIsCorrect = this.colSolutions[col] == this.getColumnTotal(col);
                UIUtils.changeSolutionBackgroundColor(colSolutionDisplay, col, colIsCorrect);
                if (gridIsCorrect) {
                    gridIsCorrect = colIsCorrect;
                }
            }
        }
        return gridIsCorrect;
    }

    static boolean isOperatorAsciiValue(int n) {
        return n == 247 ||  // '×'
                n == 43 || // '+'
                n == 45 || // '-'
                n == 215;   // '÷'
    }

    public String toString(){
        StringBuilder toReturn = new StringBuilder("   ");
        for (int i = 0; i < nailedRow.length; i++){
            toReturn.append(nailedRow[i]);
            toReturn.append(" ");
        }
        toReturn.append("\n");

        for (int row = 0; row < grid.length; row++){
            toReturn.append(nailedCol[row]);
            toReturn.append(" ");
            for (int col = 0; col < grid[0].length; col++){
                toReturn.append(grid[row][col]);
                toReturn.append(" ");
            }
            toReturn.append("\n");
        }
        return toReturn.toString();
    }

    public static void main(String[] args){
//        int[] nailedRow = new int[] {EMPTY, EMPTY, 7};
//        int[] nailedCol = new int[] {EMPTY, 2, EMPTY};
//        Object[][] grid = new Object[][]{{null, null, '-'}, {'+', null, 4},{null, null, null}};
//        int[] rowSols = new int[] {EMPTY, 6, EMPTY};
//        int[] colSols = new int[] {EMPTY, EMPTY, 3};
//        Board b = new Board(grid, rowSols, colSols, nailedRow, nailedCol, null, null);
//        System.out.println(b.toString());
    }
}
