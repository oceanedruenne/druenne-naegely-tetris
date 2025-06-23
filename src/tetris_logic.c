#define _POSIX_C_SOURCE 199309L

#include "tetris_logic.h"

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

// int main() {
//     srand(time(NULL));

//     // Initialize game and fixed grids
//     int grid[NB_BLOCS_W][NB_BLOCS_H] = {0};
//     int fixedGrid[NB_BLOCS_W][NB_BLOCS_H] = {0};

//     // Initialize a new tetromino
//     int tetromino[3][3];
//     int startX, startY;
//     generateNewTetromino(tetromino, &startX, &startY);

//     // Check whether the part can be placed at the beginning
//     if (!canPlace(tetromino, startX, startY, fixedGrid)) {
//         printf("Game Over !\n");
//         return 0;
//     }

//     struct timespec lastUpdate;
//     clock_gettime(CLOCK_MONOTONIC, &lastUpdate);

//     // Main game loop
//  while (1) {

//         // Appel à l'IA pour déterminer le mouvement à effectuer
//         char ia_move = tetris_ai_play(grid, fixedGrid, tetromino, &startX, &startY);
        
//         /*
//         char input = getNonBlockingKey();
        
//         if ((input == 'd' || input == CHAR_RIGHT) && canPlace(tetromino, startX, startY + 1, fixedGrid)) {
//             startY++;
//         } else if ((input == 'q' || input == CHAR_LEFT) && canPlace(tetromino, startX, startY - 1, fixedGrid)) {
//             startY--;
//         } else if ((input == 's' || input == CHAR_DOWN) && canPlace(tetromino, startX + 1, startY, fixedGrid)) {
//             startX++;
//         } else if (input == 'x') {
//             int temp[3][3];
//             for (int i = 0; i < 3; i++) {
//                 for (int j = 0; j < 3; j++) {
//                     temp[j][2 - i] = tetromino[i][j];
//                 }
//             }
                
//             if (canPlace(temp, startX, startY, fixedGrid)) {
//                 for (int i = 0; i < 3; i++) {
//                     for (int j = 0; j < 3; j++) {
//                         tetromino[i][j] = temp[i][j];
//                     }
//                 }
//             }
//         } else if (input == 'b') {
//             break;
//         }
//         */

//         // Traitement des mouvements déterminés par l'IA
//         if (ia_move == 'd' && canPlace(tetromino, startX, startY + 1, fixedGrid)) {
//             startY++;
//         } else if (ia_move == 'q' && canPlace(tetromino, startX, startY - 1, fixedGrid)) {
//             startY--;
//         } else if (ia_move == 's' && canPlace(tetromino, startX + 1, startY, fixedGrid)) {
//             startX++;
//         } else if (ia_move == 'x') {
//             int temp[3][3];
//             for (int i = 0; i < 3; i++) {
//                 for (int j = 0; j < 3; j++) {
//                     temp[j][2 - i] = tetromino[i][j];
//                 }
//             }
//             if (canPlace(temp, startX, startY, fixedGrid)) {
//                 memcpy(tetromino, temp, sizeof(temp));
//             }
//         }

//         // Manage the tetromino's descent speed
//         struct timespec now;
//         clock_gettime(CLOCK_MONOTONIC, &now);
//         double elapsed = (now.tv_sec - lastUpdate.tv_sec) + (now.tv_nsec - lastUpdate.tv_nsec) / 1e9;

//         if (elapsed >= 0.5) {
//             if (canPlace(tetromino, startX + 1, startY, fixedGrid)) {
//                 startX++;
//             } else {
//                 freezeTetromino(fixedGrid, tetromino, startX, startY);
//                 clearLines(fixedGrid);
//                 generateNewTetromino(tetromino, &startX, &startY);

//                 // Check if the game is finished
//                 if (!canPlace(tetromino, startX, startY, fixedGrid)) {
//                     placeTetromino(grid, fixedGrid, tetromino, startX, startY);
//                     displayGrid(grid);
//                     printf("Game Over !\n");
//                     break;
//                 }
//             }

//             lastUpdate = now;
//         }

//         placeTetromino(grid, fixedGrid, tetromino, startX, startY);
//         displayGrid(grid); // Display the updated grid
//         usleep(50 * 1000);
//     }
// }
