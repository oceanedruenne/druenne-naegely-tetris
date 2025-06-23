#include "AITetrisPlayer.h"
#include <string.h>

#define ROW 15
#define COLUMN 10

// Evaluate the position of the tetromino
int evaluatePosition(int testGrid[ROW][COLUMN]) {
    int score = 0; // Score to determine whether the tetromino is in the right place or not

    // Bonus for each complete line
    for (int i = 0; i < ROW; i++) {
        int full = 1;

        for (int j = 0; j < COLUMN; j++) {
            if (testGrid[i][j] == 0) {
                full = 0;
                break;
            }
        }

        if (full) {
            score -= 1000; // More weight here
        }
    }

    // Height penalty (we want to stack as low as possible)
    for (int j = 0; j < COLUMN; j++) {
        for (int i = 0; i < ROW; i++) {
            if (testGrid[i][j] != 0) {
                score += (ROW - i) * 2; // Lower = better
                break;
            }
        }
    }

    // Penalty for holes (empty space under a block)
    for (int j = 0; j < COLUMN; j++) {
        int seenBlock = 0;

        for (int i = 0; i < ROW; i++) {
            if (testGrid[i][j] != 0) {
                seenBlock = 1;
            } else if (seenBlock) {
                score += 50; // Big penalty for a hole
            }
        }
    }

    return score;
}

// Try to place a tetromino in a position
int tryPlace(int grid[ROW][COLUMN], int fixedGrid[ROW][COLUMN], int tetromino[3][3], int x, int y) {
    
    int testGrid[ROW][COLUMN];
    memcpy(testGrid, fixedGrid, sizeof(testGrid));

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tetromino[i][j] != 0) {
                int xx = x + i;
                int yy = y + j;

                if (xx < 0 || xx >= ROW || yy < 0 || yy >= COLUMN || testGrid[xx][yy] != 0) {
                    return 999999;
                }
                    
                testGrid[xx][yy] = tetromino[i][j];
            }
        }
    }

    return evaluatePosition(testGrid);
}

// Apply a clockwise rotation
void rotateTetromino(int src[3][3], int dest[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            dest[j][2 - i] = src[i][j];
        }
    }     
}

// Check if AI can move down the tetromino or not
int canMoveDown(int grid[ROW][COLUMN], int fixedGrid[ROW][COLUMN], int tetromino[3][3], int x, int y) {
    return (tryPlace(grid, fixedGrid, tetromino, x + 1, y) != 999999);
}

// Check if AI can move left the tetromino or not
int canMoveLeft(int grid[ROW][COLUMN], int fixedGrid[ROW][COLUMN], int tetromino[3][3], int x, int y) {
    return (tryPlace(grid, fixedGrid, tetromino, x, y - 1) != 999999);
}

// Check if AI can move right the tetromino or not 
int canMoveRight(int grid[ROW][COLUMN], int fixedGrid[ROW][COLUMN], int tetromino[3][3], int x, int y) {
    return (tryPlace(grid, fixedGrid, tetromino, x, y + 1) != 999999);
}

// Main function of AI
char tetris_ai_play(int grid[ROW][COLUMN], int fixedGrid[ROW][COLUMN], int tetromino[3][3], int *startX, int *startY) {
    int bestScore = 999999;
    int bestX = *startX;
    int bestY = *startY;

    int piece[3][3];
    memcpy(piece, tetromino, sizeof(piece));

    // Find effective width
    int right = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (piece[i][j] != 0 && j > right) right = j;

    // Test horizontal positions
    for (int y = 0; y <= COLUMN - 1 - right; y++) {
        int x = 0;
        while (tryPlace(grid, fixedGrid, piece, x + 1, y) != 999999) x++;

        int score = tryPlace(grid, fixedGrid, piece, x, y);
        if (score < bestScore) {
            bestScore = score;
            bestX = x;
            bestY = y;
        }
    }

    // Horizontal movements if possible
    if (*startY < bestY && canMoveRight(grid, fixedGrid, piece, *startX, *startY)) {
        (*startY)++;
        return 'd';
    }
    if (*startY > bestY && canMoveLeft(grid, fixedGrid, piece, *startX, *startY)) {
        (*startY)--;
        return 'q';
    }

    // Downhill if possible
    if (*startX < bestX && canMoveDown(grid, fixedGrid, piece, *startX, *startY)) {
        (*startX)++;
        return 's';
    }

    // Otherwise, place the tetromino
    return ' ';
}
