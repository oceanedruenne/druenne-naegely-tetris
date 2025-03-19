#include <stdio.h>
// #include "Tetromino.h"

#define ROW 22
#define COLUMN 10

void afficherGrille(int grid[ROW][COLUMN]) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int grid[ROW][COLUMN] = {0}; 

    int piece[3][3] = {
        {0, 4, 4},
        {4, 4, 0},
        {0, 0, 0}
    };

    int startX = 0, startY = 3; 

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (piece[i][j] != 0) { 
                grid[startX + i][startY + j] = piece[i][j];
            }
        }
    }

    afficherGrille(grid);

    return 0;
}