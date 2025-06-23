#ifndef TETRIS_AI_H
#define TETRIS_AI_H

#include "tetromino.h"
#include "tetris_logic.h"

char tetris_ai_play(int grid[NB_BLOCS_W][NB_BLOCS_H], int fixedGrid[NB_BLOCS_W][NB_BLOCS_H],
                    int tetromino[3][3], int *startX, int *startY);

#endif
