package com.example.miles.myapplication;

import static com.example.miles.myapplication.Board.EMPTY;

class LevelInfo {

    // TODO: Update when we add another level
    static final int NUM_LEVELS_IMPLEMENTED = 7;
    static final int UNKNOWN = 0;

    private int levelNumber;

    int[][] grid;
    int[] rowSolutions;
    int[] colSolutions;
    int[] nailedRow;
    int[] nailedCol;
    int moves2Stars;
    int moves3Stars;

    LevelInfo(int levelNumber){
        this.levelNumber = levelNumber;
        getLevelData();
    }

    private void getLevelData() {
        switch (levelNumber) {
            case 1:
                grid = new int[][]{{EMPTY, EMPTY, 9}, {EMPTY, EMPTY,'-'}, {4,'+',1}};
                rowSolutions = new int[] {13, EMPTY, EMPTY};
                colSolutions = new int[]{3, EMPTY, EMPTY};
                nailedRow = new int[] {EMPTY, EMPTY, EMPTY};
                nailedCol = new int[] {EMPTY, EMPTY, EMPTY};
                moves2Stars = 8;
                moves3Stars = 4;
                break;

            case 2:
                grid = new int[][]{{EMPTY, EMPTY, 4},{EMPTY, EMPTY, EMPTY},{EMPTY, '+', '-'}};
                rowSolutions = new int[] {EMPTY, 6, EMPTY};
                colSolutions = new int[]{EMPTY, EMPTY, 3};
                nailedRow = new int[] {EMPTY, EMPTY, 7};
                nailedCol = new int[] {EMPTY, 2, EMPTY};
                moves2Stars = 11;
                moves3Stars = 7;
                break;

            case 3:
                grid = new int[][]{{3, EMPTY, '÷'},{'×', EMPTY, EMPTY},{2, 6, EMPTY}};
                rowSolutions = new int[] {12, EMPTY, EMPTY};
                colSolutions = new int[]{2, EMPTY, EMPTY};
                nailedRow = new int[] {EMPTY, EMPTY, EMPTY};
                nailedCol = new int[] {EMPTY, EMPTY, EMPTY};
                moves2Stars = 13;
                moves3Stars = 9;
                break;

            case 4:
                grid = new int[][]{{2, EMPTY, '-'},{EMPTY, '×', EMPTY},{3, EMPTY, '+'}};
                rowSolutions = new int[] {EMPTY, 5, EMPTY};
                colSolutions = new int[]{2, EMPTY, 4};
                nailedRow = new int[] {5, EMPTY, 2};
                nailedCol = new int[] {EMPTY, EMPTY, EMPTY};
                moves2Stars = 12;
                moves3Stars = 9;
                break;

            case 5:
                grid = new int[][]{{3, '÷', 5},{EMPTY, EMPTY, EMPTY},{'-', 4, 6}};
                rowSolutions = new int[] {EMPTY, 1, 2};
                colSolutions = new int[]{EMPTY, 1, EMPTY};
                nailedRow = new int[] {EMPTY, EMPTY, EMPTY};
                nailedCol = new int[] {EMPTY, EMPTY, 6};
                moves2Stars = 18;
                moves3Stars = 14;
                break;

            case 6:
                grid = new int[][]{{1, 8, '+'},{EMPTY, 2, 4},{'-', '-', '÷'}};
                rowSolutions = new int[] {4, EMPTY, 3};
                colSolutions = new int[]{4, EMPTY, 3};
                nailedRow = new int[] {EMPTY, EMPTY, EMPTY};
                nailedCol = new int[] {EMPTY, EMPTY, EMPTY};
                moves2Stars = 33;
                moves3Stars = 21;
                break;

            case 7:
                grid = new int[][]{{'+', 3, '×'}, {3, 4, 5}, {EMPTY, EMPTY, '÷'}};
                rowSolutions = new int[] {15, EMPTY, EMPTY};
                colSolutions = new int[]{1,EMPTY,9};
                nailedRow = new int[] {EMPTY, EMPTY, EMPTY};
                nailedCol = new int[] {EMPTY, EMPTY, EMPTY};
                moves2Stars = 20;
                moves3Stars = 12;
        }
    }
}
