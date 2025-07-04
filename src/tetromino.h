#ifndef TETROMINO_H
#define TETROMINO_H

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

struct coords { int x; int y; };

typedef struct Tetromino {
  int shape[4][4];
  char type;
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

typedef struct TetrominoColor {
  int r;
  int g;
  int b;
  int a;
} TetrominoColor;

typedef struct TetrominoColorCollection {
  struct TetrominoColor I;
  struct TetrominoColor J;
  struct TetrominoColor L;
  struct TetrominoColor O;
  struct TetrominoColor S;
  struct TetrominoColor T;
  struct TetrominoColor Z;
} TetrominoColorCollection;


TetrominoCollection initTetrominoCollection();

TetrominoColorCollection initTetrominoColorCollection();

Tetromino getRandomTetromino(TetrominoCollection* collection);

TetrominoColor getTetrominoColor(int color_number);

void rotate_tetromino(Tetromino *tetromino);

#endif
