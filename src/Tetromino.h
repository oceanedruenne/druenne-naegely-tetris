#include <stdlib.h>
#include <time.h>

struct coords { int x; int y; };

typedef struct Tetromino {
  int shape[4][4];
  char type;
  int r;
  int g;
  int b;
  int a;
  struct coords position;
} Tetromino;

typedef struct TetrominoCollection {
  struct Tetromino I;
  struct Tetromino J;
  struct Tetromino L;
  struct Tetromino O;
  struct Tetromino S;
  struct Tetromino T;
  struct Tetromino Z;
} TetrominoCollection;

TetrominoCollection initTetrominoCollection();

Tetromino getRandomTetromino(TetrominoCollection* collection);

void printTetromino(Tetromino tetromino);


