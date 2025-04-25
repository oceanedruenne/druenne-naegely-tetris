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

char getchNonBloquant() {
    struct termios oldt, newt;
    char ch = 0;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
        read(STDIN_FILENO, &ch, 1);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void afficherGrille(int grid[ROW][COLUMN]) {
    printf("\033[H\033[J");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (grid[i][j] == 0)
                printf(". ");
            else
                printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

int peutPlacer(int piece[3][3], int startX, int startY, int grilleFixe[ROW][COLUMN]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (piece[i][j] != 0) {
                int x = startX + i;
                int y = startY + j;
                if (x < 0 || x >= ROW || y < 0 || y >= COLUMN)
                    return 0;
                if (grilleFixe[x][y] != 0)
                    return 0;
            }
        }
    }
    return 1;
}

void placerPiece(int grid[ROW][COLUMN], int grilleFixe[ROW][COLUMN], int piece[3][3], int startX, int startY) {
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COLUMN; j++)
            grid[i][j] = grilleFixe[i][j];

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (piece[i][j] != 0)
                grid[startX + i][startY + j] = piece[i][j];
}

void figerPiece(int grilleFixe[ROW][COLUMN], int piece[3][3], int startX, int startY) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (piece[i][j] != 0)
                grilleFixe[startX + i][startY + j] = piece[i][j];
}

void effacerLignes(int grilleFixe[ROW][COLUMN]) {
    for (int i = ROW - 1; i >= 0; i--) {
        int pleine = 1;
        for (int j = 0; j < COLUMN; j++) {
            if (grilleFixe[i][j] == 0) {
                pleine = 0;
                break;
            }
        }
        if (pleine) {
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < COLUMN; j++) {
                    grilleFixe[k][j] = grilleFixe[k - 1][j];
                }
            }
            for (int j = 0; j < COLUMN; j++)
                grilleFixe[0][j] = 0;
            i++; // Revérifier la ligne actuelle après décalage
        }
    }
}

void genererNouvellePiece(int piece[3][3], int *startX, int *startY) {
    int type = rand() % 5;
    int nouvelle[3][3] = {0};

    switch (type) {
        case 0: {
            int s[3][3] = {{0, 0, 0}, {0, 5, 5}, {5, 5, 0}};
            memcpy(nouvelle, s, sizeof(s));
            break;
        }
        case 1: {
            int z[3][3] = {{0, 0, 0}, {7, 7, 0}, {0, 7, 7}};
            memcpy(nouvelle, z, sizeof(z));
            break;
        }
        case 2: {
            int t[3][3] = {{0, 0, 0}, {6, 6, 6}, {0, 6, 0}};
            memcpy(nouvelle, t, sizeof(t));
            break;
        }
        case 3: {
            int j[3][3] = {{0, 0, 0}, {2, 2, 2}, {0, 0, 2}};
            memcpy(nouvelle, j, sizeof(j));
            break;
        }
        case 4: {
            int l[3][3] = {{0, 0, 0}, {3, 3, 3}, {3, 0, 0}};
            memcpy(nouvelle, l, sizeof(l));
            break;
        }
    }

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            piece[i][j] = nouvelle[i][j];

    *startX = 0;
    *startY = 3;
}

int main() {
    srand(time(NULL));

    int grid[ROW][COLUMN] = {0};
    int grilleFixe[ROW][COLUMN] = {0};

    int piece[3][3];
    int startX, startY;
    genererNouvellePiece(piece, &startX, &startY);

    if (!peutPlacer(piece, startX, startY, grilleFixe)) {
        printf("Game Over !\n");
        return 0;
    }

    struct timespec lastUpdate;
    clock_gettime(CLOCK_MONOTONIC, &lastUpdate);

    while (1) {
        char input = getchNonBloquant();

        if (input == 'd' && peutPlacer(piece, startX, startY + 1, grilleFixe)) {
            startY++;
        } else if (input == 'q' && peutPlacer(piece, startX, startY - 1, grilleFixe)) {
            startY--;
        } else if (input == 's' && peutPlacer(piece, startX + 1, startY, grilleFixe)) {
            startX++;
        } else if (input == 'x') {
            int temp[3][3];
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    temp[j][2 - i] = piece[i][j];

            if (peutPlacer(temp, startX, startY, grilleFixe)) {
                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++)
                        piece[i][j] = temp[i][j];
            }
        } else if (input == 'b') {
            break;
        }

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed = (now.tv_sec - lastUpdate.tv_sec) + (now.tv_nsec - lastUpdate.tv_nsec) / 1e9;

        if (elapsed >= 0.5) {
            if (peutPlacer(piece, startX + 1, startY, grilleFixe)) {
                startX++;
            } else {
                figerPiece(grilleFixe, piece, startX, startY);
                effacerLignes(grilleFixe);
                genererNouvellePiece(piece, &startX, &startY);
                if (!peutPlacer(piece, startX, startY, grilleFixe)) {
                    afficherGrille(grid);
                    printf("Game Over !\n");
                    break;
                }
            }
            lastUpdate = now;
        }

        placerPiece(grid, grilleFixe, piece, startX, startY);
        afficherGrille(grid);
        usleep(50 * 1000);
    }

    return 0;
}
