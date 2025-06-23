#include "tetromino.h"
#include "tetris_logic.h"


#ifndef TETRIS_AI_H
#define TETRIS_AI_H

char tetris_ai_play(int grid[ROW][COLUMN], int fixedGrid[ROW][COLUMN],
                    int tetromino[3][3], int *startX, int *startY);
#endif
