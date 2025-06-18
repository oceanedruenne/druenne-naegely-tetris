#ifndef TETRIS_LOGIC_H
#define TETRIS_LOGIC_H

#define ROW 15
#define COLUMN 10

#define CHAR_DOWN  -2
#define CHAR_LEFT  -3
#define CHAR_RIGHT -4


int canPlace(int tetromino[3][3], int x, int y, int fixedGrid[ROW][COLUMN]);
void freezeTetromino(int fixedGrid[ROW][COLUMN], int tetromino[3][3], int x, int y);
void clearLines(int fixedGrid[ROW][COLUMN]);
void generateNewTetromino(int tetromino[3][3], int *startX, int *startY);

#endif