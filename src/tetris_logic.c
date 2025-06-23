#define _POSIX_C_SOURCE 199309L

#include "tetris_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include "AITetrisPlayer.h"
#include "tetromino.h"

// Function for obtaining a key without blocking
char getNonBlockingKey() {
    struct termios oldt, newt;
    char ch = 0;
    char seq[3];

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
        if (read(STDIN_FILENO, &seq[0], 1) == 1) {
            if (seq[0] == '\033') { // début séquence
                if (read(STDIN_FILENO, &seq[1], 1) == 1 &&
                    read(STDIN_FILENO, &seq[2], 1) == 1) {
                    if (seq[1] == '[') {
                        switch (seq[2]) {
                            case 'B': ch = CHAR_DOWN; break;
                            case 'C': ch = CHAR_RIGHT; break;
                            case 'D': ch = CHAR_LEFT; break;
                        }
                    }
                }
            } else {
                ch = seq[0]; 
            }
        }
    }

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
int clearLines(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W]) {
    int cleared_lines = 0;
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
            cleared_lines++;
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
    return cleared_lines;
}

void init_game_state(GameState *state) {
    state->level = 1;
    state->total_lines_cleared = 0;
    state->lines_cleared_for_level = 0;
}

void update_game_state(GameState *state, int cleared_lines) {
    if (cleared_lines > 0) {
        state->total_lines_cleared += cleared_lines;
        state->lines_cleared_for_level += cleared_lines;
        if (state->lines_cleared_for_level >= 10) {
            state->level++;
            state->lines_cleared_for_level -= 10;
        }
    }
}

bool try_rotate_tetromino(Tetromino *tetromino, int *x, int *y, int fixedGrid[NB_BLOCS_H][NB_BLOCS_W]) {
    Tetromino temp = *tetromino;
    rotate_tetromino(&temp);

    if (canPlace(temp, *x, *y, fixedGrid)) {
        *tetromino = temp;
        return true;
    }

    // Try wall kick (move left)
    if (canPlace(temp, *x, *y - 1, fixedGrid)) {
        *tetromino = temp;
        *y -= 1;
        return true;
    }

    // Try wall kick (move right)
    if (canPlace(temp, *x, *y + 1, fixedGrid)) {
        *tetromino = temp;
        *y += 1;
        return true;
    }

    return false;
}

void add_garbage_lines(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], int line_count) {
    // Shift existing blocks up
    for (int i = 0; i < NB_BLOCS_H - line_count; i++) {
        for (int j = 0; j < NB_BLOCS_W; j++) {
            fixedGrid[i][j] = fixedGrid[i + line_count][j];
        }
    }

    // Add new garbage lines at the bottom
    for (int i = NB_BLOCS_H - line_count; i < NB_BLOCS_H; i++) {
        int empty_spot = rand() % NB_BLOCS_W;
        for (int j = 0; j < NB_BLOCS_W; j++) {
            if (j == empty_spot) {
                fixedGrid[i][j] = 0;
            } else {
                fixedGrid[i][j] = 8; // Garbage block type
            }
        }
    }
}

