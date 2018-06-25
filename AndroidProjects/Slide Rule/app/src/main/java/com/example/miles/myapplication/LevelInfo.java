package com.example.miles.myapplication;

import static com.example.miles.myapplication.Board.EMPTY;

class LevelInfo {

    // TODO: Update when we add another level
    static final int NUM_LEVELS_IMPLEMENTED = 3;

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
        }
    }
}
