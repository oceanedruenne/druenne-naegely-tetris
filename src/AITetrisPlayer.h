#ifndef TETRIS_AI_H
#define TETRIS_AI_H

#include "tetromino.h"
#include "tetris_logic.h"

char tetris_ai_play(int grid[NB_BLOCS_H][NB_BLOCS_W], int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int *startX, int *startY);
int evaluate_position(int grid[NB_BLOCS_H][NB_BLOCS_W]);
int try_place_for_score(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int x, int y);

#endif
