#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <string.h>

#define ROW 15
#define COLUMN 10

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
void displayGrid(int grid[ROW][COLUMN]) {

    printf("\033[H\033[J");

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
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
int canPlace(int tetromino[3][3], int startX, int startY, int fixedGrid[ROW][COLUMN]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tetromino[i][j] != 0) {
                int x = startX + i;
                int y = startY + j;

                // Checks whether the part exceeds the limits of the grid
                if (x < 0 || x >= ROW || y < 0 || y >= COLUMN) {
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
void placeTetromino(int grid[ROW][COLUMN], int fixedGrid[ROW][COLUMN], int tetromino[3][3], int startX, int startY) {
    
    // Copying the fixed grid into the game grid
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            grid[i][j] = fixedGrid[i][j];
        }
    } 

    // Place the tetromino on the grid
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tetromino[i][j] != 0) {
                grid[startX + i][startY + j] = tetromino[i][j];
            }
        }
    }    
}

// Function that drags the part into the fixed grid
void freezeTetromino(int fixedGrid[ROW][COLUMN], int tetromino[3][3], int startX, int startY) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tetromino[i][j] != 0) {
                fixedGrid[startX + i][startY + j] = tetromino[i][j];
            }
        }
    }     
}

// Function that deletes complete grid lines
void clearLines(int fixedGrid[ROW][COLUMN]) {
    for (int i = ROW - 1; i >= 0; i--) {
        int full = 1;

        // Checks if the line is full
        for (int j = 0; j < COLUMN; j++) {
            if (fixedGrid[i][j] == 0) {
                full = 0;
                break;
            }
        }

        // If the line is full, move it downwards
        if (full) {
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < COLUMN; j++) {
                    fixedGrid[k][j] = fixedGrid[k - 1][j];
                }
            }

            // Fill the first line with zeros
            for (int j = 0; j < COLUMN; j++) {
                fixedGrid[0][j] = 0;
            }

            i++; 
        }
    }
}

// Function that generates a new random tetromino
void generateNewTetromino(int tetromino[3][3], int *startX, int *startY) {
    int tetrominoType = rand() % 5; // Choose a random tetromino type
    int newTetromino[3][3] = {0};

    // Define the shape of the part according to its type
    switch (tetrominoType) {
        case 0: {
            int s[3][3] = {{0, 0, 0}, {0, 5, 5}, {5, 5, 0}};
            memcpy(newTetromino, s, sizeof(s));
            break;
        }
        case 1: {
            int z[3][3] = {{0, 0, 0}, {7, 7, 0}, {0, 7, 7}};
            memcpy(newTetromino, z, sizeof(z));
            break;
        }
        case 2: {
            int t[3][3] = {{0, 0, 0}, {6, 6, 6}, {0, 6, 0}};
            memcpy(newTetromino, t, sizeof(t));
            break;
        }
        case 3: {
            int j[3][3] = {{0, 0, 0}, {2, 2, 2}, {0, 0, 2}};
            memcpy(newTetromino, j, sizeof(j));
            break;
        }
        case 4: {
            int l[3][3] = {{0, 0, 0}, {3, 3, 3}, {3, 0, 0}};
            memcpy(newTetromino, l, sizeof(l));
            break;
        }
    }

    // Copy the new tetromino into the variable piece
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            tetromino[i][j] = newTetromino[i][j];
        }
    }
        
    *startX = 0;
    *startY = 3;
}

int main() {
    srand(time(NULL));

    // Initialize game and fixed grids
    int grid[ROW][COLUMN] = {0};
    int fixedGrid[ROW][COLUMN] = {0};

    // Initialize a new tetromino
    int tetromino[3][3];
    int startX, startY;
    generateNewTetromino(tetromino, &startX, &startY);

    // Check whether the part can be placed at the beginning
    if (!canPlace(tetromino, startX, startY, fixedGrid)) {
        printf("Game Over !\n");
        return 0;
    }

    struct timespec lastUpdate;
    clock_gettime(CLOCK_MONOTONIC, &lastUpdate);

    // Main game loop
    while (1) {

        char input = getNonBlockingKey();
        
        // Move the tetromino according to the keys pressed
        if (input == 'd' && canPlace(tetromino, startX, startY + 1, fixedGrid)) {
            startY++;
        } else if (input == 'q' && canPlace(tetromino, startX, startY - 1, fixedGrid)) {
            startY--;
        } else if (input == 's' && canPlace(tetromino, startX + 1, startY, fixedGrid)) {
            startX++;
        } else if (input == 'x') {
            int temp[3][3];
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    temp[j][2 - i] = tetromino[i][j];
                }
            }
                
            if (canPlace(temp, startX, startY, fixedGrid)) {
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        tetromino[i][j] = temp[i][j];
                    }
                }
            }
        } else if (input == 'b') {
            break;
        }

        // Manage the tetromino's descent speed
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed = (now.tv_sec - lastUpdate.tv_sec) + (now.tv_nsec - lastUpdate.tv_nsec) / 1e9;

        if (elapsed >= 0.5) {
            if (canPlace(tetromino, startX + 1, startY, fixedGrid)) {
                startX++;
            } else {
                freezeTetromino(fixedGrid, tetromino, startX, startY);
                clearLines(fixedGrid);
                generateNewTetromino(tetromino, &startX, &startY);

                // Check if the game is finished
                if (!canPlace(tetromino, startX, startY, fixedGrid)) {
                    displayGrid(grid);
                    printf("Game Over !\n");
                    break;
                }
            }

            lastUpdate = now;
        }

        placeTetromino(grid, fixedGrid, tetromino, startX, startY);
        displayGrid(grid); // Display the updated grid
        usleep(50 * 1000);
    }
}