#define _POSIX_C_SOURCE 199309L

#include "tetris_logic.h"

// Function for obtaining a key without blocking
char getNonBlockingKey() {
    struct termios oldt, newt;
    char ch = 0;

    // Retrieving terminal parameters
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);

    // Modification of the terminal to read it without blocking
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    struct timeval tv = {0L, 0L};
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    // Check whether a key has been pressed
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
        // Read the key that has been pressed
        read(STDIN_FILENO, &ch, 1);
    }

    // Restoring the terminal's initial settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

// Function for displaying the game grid
void displayGrid(int grid[NB_BLOCS_H][NB_BLOCS_W]) {

    printf("\033[H\033[J");

    for (int i = 0; i < NB_BLOCS_H; i++) {
        for (int j = 0; j < NB_BLOCS_W; j++) {
            if (grid[i][j] == 0) {
                printf(". ");
            } else {
                printf("%d ", grid[i][j]);
            } 
        }

        printf("\n");
    }
}

// Function that checks whether a tetromino can be placed in the given position
int canPlace(Tetromino tetromino, int startX, int startY, int fixedGrid[NB_BLOCS_H][NB_BLOCS_W]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetromino.shape[i][j] != 0) {
                int x = startX + i;
                int y = startY + j;

                // Checks whether the part exceeds the limits of the grid
                if (x < 0 || x >= NB_BLOCS_H || y < 0 || y >= NB_BLOCS_W) {
                    return 0;
                }

                // Checks if the box is already occupied    
                if (fixedGrid[x][y] != 0) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

// Function that places the tetromino on the grid
void placeTetromino(int grid[NB_BLOCS_H][NB_BLOCS_W], int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int startX, int startY) {
    
    // Copying the fixed grid into the game grid
    for (int i = 0; i < NB_BLOCS_H; i++) {
        for (int j = 0; j < NB_BLOCS_W; j++) {
            grid[i][j] = fixedGrid[i][j];
        }
    } 

    // Place the tetromino on the grid
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetromino.shape[i][j] != 0) {
                grid[startX + i][startY + j] = tetromino.shape[i][j];
            }
        }
    }    
}

// Function that drags the part into the fixed grid
void freezeTetromino(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int startX, int startY) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetromino.shape[i][j] != 0) {
                fixedGrid[startX + i][startY + j] = tetromino.shape[i][j];
            }
        }
    }     
}

// Function that deletes complete grid lines
void clearLines(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W]) {
    for (int i = NB_BLOCS_H - 1; i >= 0; i--) {
        int full = 1;

        // Checks if the line is full
        for (int j = 0; j < NB_BLOCS_W; j++) {
            if (fixedGrid[i][j] == 0) {
                full = 0;
                break;
            }
        }

        // If the line is full, move it downwards
        if (full) {
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < NB_BLOCS_W; j++) {
                    fixedGrid[k][j] = fixedGrid[k - 1][j];
                }
            }

            // Fill the first line with zeros
            for (int j = 0; j < NB_BLOCS_W; j++) {
                fixedGrid[0][j] = 0;
            }

            i++; 
        }
    }
}
