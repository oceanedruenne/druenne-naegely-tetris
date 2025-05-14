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
#include "tetromino.h"

#define NB_BLOCS_H 21
#define NB_BLOCS_W 10
#define SLEEP_TIME 1000

char getNonBlockingKey();
void displayGrid(int grid[NB_BLOCS_H][NB_BLOCS_W]);
int canPlace(Tetromino tetromino, int startX, int startY, int fixedGrid[NB_BLOCS_H][NB_BLOCS_W]);
void placeTetromino(int grid[NB_BLOCS_H][NB_BLOCS_W], int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int startX, int startY);
void freezeTetromino(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W], Tetromino tetromino, int startX, int startY);
void clearLines(int fixedGrid[NB_BLOCS_H][NB_BLOCS_W]);

#endif
