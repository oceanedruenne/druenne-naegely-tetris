#include "AITetrisPlayer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> // For abs

// Forward declaration
int evaluate_position(int grid[NB_BLOCS_H][NB_BLOCS_W]);

// Tries to place a tetromino and returns a score. A high score is bad.
int try_place_for_score(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int x, int y) {
    int testGrid[NB_BLOCS_H][NB_BLOCS_W];
    memcpy(testGrid, fixedGrid, sizeof(testGrid));

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetromino.shape[i][j] != 0) {
                int newX = x + i;
                int newY = y + j;
                if (newX >= NB_BLOCS_H || newY < 0 || newY >= NB_BLOCS_W || testGrid[newX][newY] != 0) {
                    return 999999; // Invalid move, return a very high score
                }
                testGrid[newX][newY] = tetromino.shape[i][j];
            }
        }
    }
    return evaluate_position(testGrid);
}


// Evaluates the grid state. Lower score is better.
int evaluate_position(int grid[NB_BLOCS_H][NB_BLOCS_W]) {
    int score = 0;
    int heights[NB_BLOCS_W] = {0};
    int maxHeight = 0;

    // Calculate heights of each column
    for (int j = 0; j < NB_BLOCS_W; j++) {
        for (int i = 0; i < NB_BLOCS_H; i++) {
            if (grid[i][j] != 0) {
                heights[j] = NB_BLOCS_H - i;
                break;
            }
        }
        if (heights[j] > maxHeight) {
            maxHeight = heights[j];
        }
    }

    // Aggregate height - sum of all column heights
    int totalHeight = 0;
    for(int j=0; j < NB_BLOCS_W; j++) {
        totalHeight += heights[j];
    }
    score += totalHeight * 10;

    // Holes - empty cells with a block above them
    int holes = 0;
    for (int j = 0; j < NB_BLOCS_W; j++) {
        int block_seen = 0;
        for (int i = 0; i < NB_BLOCS_H; i++) {
            if (grid[i][j] != 0) {
                block_seen = 1;
            } else if (block_seen) {
                holes++;
            }
        }
    }
    score += holes * 50;

    // Bumpiness - sum of absolute differences between adjacent column heights
    int bumpiness = 0;
    for (int j = 0; j < NB_BLOCS_W - 1; j++) {
        bumpiness += abs(heights[j] - heights[j+1]);
    }
    score += bumpiness * 5;

    // Completed lines - big bonus for clearing lines
    int completed_lines = 0;
    for (int i = 0; i < NB_BLOCS_H; i++) {
        int is_line_complete = 1;
        for (int j = 0; j < NB_BLOCS_W; j++) {
            if (grid[i][j] == 0) {
                is_line_complete = 0;
                break;
            }
        }
        if (is_line_complete) {
            completed_lines++;
        }
    }
    score -= completed_lines * 100;

    return score;
}


char tetris_ai_play(int grid[NB_BLOCS_H][NB_BLOCS_W], int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int *startX, int *startY) {
    int bestScore = 999999;
    Tetromino bestTetromino = tetromino;
    int bestX = -1, bestY = -1;

    Tetromino currentTetromino = tetromino;

    // Iterate through all 4 possible rotations
    for (int r = 0; r < 4; r++) {
        // Iterate through all possible horizontal positions
        for (int y = -3; y < NB_BLOCS_W; y++) {
            // Find where the piece would land by dropping it
            int x = 0;
            while (canPlace(currentTetromino, x + 1, y, fixedGrid)) {
                x++;
            }
            
            if (canPlace(currentTetromino, x, y, fixedGrid)) {
                int score = try_place_for_score(fixedGrid, currentTetromino, x, y);
                if (score < bestScore) {
                    bestScore = score;
                    bestTetromino = currentTetromino;
                    bestX = x;
                    bestY = y;
                }
            }
        }
        rotate_tetromino(&currentTetromino); // Rotate for the next iteration
    }

    // Now, figure out the next move to get to the best state
    
    // 1. Is the current rotation the best rotation?
    if (memcmp(tetromino.shape, bestTetromino.shape, sizeof(tetromino.shape)) != 0) {
        return 'x'; // Rotate
    }

    // 2. Is the horizontal position correct?
    if (*startY < bestY && canPlace(tetromino, *startX, *startY + 1, fixedGrid)) {
        return 'd'; // Move right
    }
    if (*startY > bestY && canPlace(tetromino, *startX, *startY - 1, fixedGrid)) {
        return 'q'; // Move left
    }

    // 3. If rotation and horizontal are correct, move down
    return 's';
}
