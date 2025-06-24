#ifndef TETRIS_LOGIC_H
#define TETRIS_LOGIC_H

#include <time.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <stdbool.h>
#include "tetromino.h"
#include <ncurses.h>

#define NB_BLOCS_H 21
#define NB_BLOCS_W 10

typedef struct GameState {
    int level;
    int total_lines_cleared;
    int lines_cleared_for_level;
} GameState;

int canPlace(Tetromino tetromino, int startX, int startY, int fixedGrid[NB_BLOCS_H][NB_BLOCS_W]);
void placeTetromino(int grid[NB_BLOCS_H][NB_BLOCS_W], int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int startX, int startY);
void freezeTetromino(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int startX, int startY);
int clearLines(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W]);
void init_game_state(GameState *state);
void update_game_state(GameState *state, int cleared_lines);
void add_garbage_lines(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], int line_count);
bool try_rotate_tetromino(Tetromino *tetromino, int *x, int *y, int fixedGrid[NB_BLOCS_H][NB_BLOCS_W]);


#endif
