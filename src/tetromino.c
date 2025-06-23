#include "tetromino.h"

TetrominoCollection initTetrominoCollection() {
    TetrominoCollection collection;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            collection.I.shape[i][j] = 0;
            collection.J.shape[i][j] = 0;
            collection.L.shape[i][j] = 0;
            collection.O.shape[i][j] = 0;
            collection.S.shape[i][j] = 0;
            collection.T.shape[i][j] = 0;
            collection.Z.shape[i][j] = 0;
        }
    }
    
    // Initialize Tetromino I
    collection.I.shape[0][0] = 1;
    collection.I.shape[0][1] = 1;
    collection.I.shape[0][2] = 1;
    collection.I.shape[0][3] = 1;
    collection.I.type = 'I';
    
    // Initialize Tetromino J
    collection.J.shape[0][0] = 2;
    collection.J.shape[1][0] = 2;
    collection.J.shape[1][1] = 2;
    collection.J.shape[1][2] = 2;
    collection.J.type = 'J';
    
    // Initialize Tetromino L
    collection.L.shape[0][2] = 3;
    collection.L.shape[1][0] = 3;
    collection.L.shape[1][1] = 3;
    collection.L.shape[1][2] = 3;
    collection.L.type = 'L';

    // Initialize Tetromino O
    collection.O.shape[0][0] = 4;
    collection.O.shape[0][1] = 4;
    collection.O.shape[1][0] = 4;
    collection.O.shape[1][1] = 4;
    collection.O.type = 'O';

    // Initialize Tetromino S
    collection.S.shape[0][1] = 5;
    collection.S.shape[0][2] = 5;
    collection.S.shape[1][0] = 5;
    collection.S.shape[1][1] = 5;
    collection.S.type = 'S';

    // Initialize Tetromino T
    collection.T.shape[0][1] = 6;
    collection.T.shape[1][0] = 6;
    collection.T.shape[1][1] = 6;
    collection.T.shape[1][2] = 6;
    collection.T.type = 'T';

    // Initialize Tetromino Z
    collection.Z.shape[0][0] = 7;
    collection.Z.shape[0][1] = 7;
    collection.Z.shape[1][1] = 7;
    collection.Z.shape[1][2] = 7;
    collection.Z.type = 'Z';

    return collection;
}

TetrominoColorCollection initTetrominoColorCollection() {
    TetrominoColorCollection collection;

    collection.I.r = 0;
    collection.I.g = 255;
    collection.I.b = 255;
    collection.I.a = 255;

    collection.J.r = 0;
    collection.J.g = 0;
    collection.J.b = 255;
    collection.J.a = 255;

    collection.L.r = 255;
    collection.L.g = 165;
    collection.L.b = 0;
    collection.L.a = 255;

    collection.O.r = 255;
    collection.O.g = 255;
    collection.O.b = 0;
    collection.O.a = 255;

    collection.S.r = 0;
    collection.S.g = 255;
    collection.S.b = 0;
    collection.S.a = 255;

    collection.T.r = 128;
    collection.T.g = 0;
    collection.T.b = 128;
    collection.T.a = 255;

    collection.Z.r = 255;
    collection.Z.g = 0;
    collection.Z.b = 0;
    collection.Z.a = 255;

    return collection;
}

Tetromino getRandomTetromino(TetrominoCollection* collection) {
    srand(time(NULL));
    int randomIndex = rand() % 7;

    switch (randomIndex) {
        case 0:
            return collection->I;
        case 1:
            return collection->J;
        case 2:
            return collection->L;
        case 3:
            return collection->O;
        case 4:
            return collection->S;
        case 5:
            return collection->T;
        case 6:
            return collection->Z;
        default:
            return collection->I;
    }
}

TetrominoColor getTetrominoColor(int color_number) {
    TetrominoColorCollection collection = initTetrominoColorCollection();
    switch (color_number) {
        case 1:
            return collection.I;
        case 2:
            return collection.J;
        case 3:
            return collection.L;
        case 4:
            return collection.O;
        case 5:
            return collection.S;
        case 6:
            return collection.T;
        case 7:
            return collection.Z;
        default:
            return collection.I;
    }
}

void printTetromino(Tetromino tetromino) {
    printf("Tetromino type: %c\n", tetromino.type);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", tetromino.shape[i][j]);
        }
        printf("\n");
    }
}
