#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#define ROW 15
#define COLUMN 10

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void afficherGrille(int grid[ROW][COLUMN]) {
    printf("\033[H\033[J"); // Efface l'écran
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

void placerPiece(int grid[ROW][COLUMN], int piece[3][3], int startX, int startY) {
    // Efface la grille
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COLUMN; j++)
            grid[i][j] = 0;

    // Place la pièce
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (piece[i][j] != 0) { 
                grid[startX + i][startY + j] = piece[i][j];
            }
        }
    }
}

int main() {
    int grid[ROW][COLUMN] = {0}; 

    int piece[3][3] = {
        {0, 4, 4},
        {4, 4, 0},
        {0, 0, 0}
    };

    int startX = 0, startY = 3; // Position initiale de la pièce

    placerPiece(grid, piece, startX, startY);
    afficherGrille(grid);

    char input;
    while (1) {
        input = getch();

        // Gestion des déplacements
        if (input == 'd' || input == 27) {  // Droite (d ou flèche droite)
            if (startY + 3 < COLUMN) startY++;
        } else if (input == 'q' || input == 26) {  // Gauche (q ou flèche gauche)
            if (startY > 0) startY--;
        } else if (input == 's' || input == 25) {  // Bas (s ou flèche bas)
            if (startX + 3 < ROW) startX++;
        } else if (input == 'b') {  // Quitter avec 'q'
            break;
        }

        placerPiece(grid, piece, startX, startY);
        afficherGrille(grid);
    }

    return 0;
}
