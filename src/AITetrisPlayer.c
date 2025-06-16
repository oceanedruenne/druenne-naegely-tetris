#include "AITetrisPlayer.h"
#include <limits.h>
#include <string.h>
#include "Tetromino.h"


typedef struct {
    int x;
    int rotation;
    int score;
} Move;

void copy_grid(int dest[ROW][COLUMN], int src[ROW][COLUMN]) {
    for (int y = 0; y < ROW; y++)
        for (int x = 0; x < COLUMN; x++)
            dest[y][x] = src[y][x];
}

void apply_tetromino(int grid[ROW][COLUMN], Tetromino *t) {
    for (int i = 0; i < 4; i++) {
        int x = t->blocks[i].x;
        int y = t->blocks[i].y;
        if (x >= 0 && x < COLUMN && y >= 0 && y < ROW) {
            grid[y][x] = t->type;
        }
    }
}

int evaluate_grid(int grid[ROW][COLUMN]) {
    int score = 0;
    int aggregate_ROW = 0;
    int holes = 0;
    int complete_lines = 0;

    for (int x = 0; x < COLUMN; x++) {
        int found_block = 0;
        for (int y = 0; y < ROW; y++) {
            if (grid[y][x]) {
                if (!found_block) {
                    aggregate_ROW += ROW - y;
                    found_block = 1;
                }
            } else if (found_block) {
                holes++;
            }
        }
    }

    for (int y = 0; y < ROW; y++) {
        int full = 1;
        for (int x = 0; x < COLUMN; x++) {
            if (!grid[y][x]) {
                full = 0;
                break;
            }
        }
        if (full) complete_lines++;
    }

    return complete_lines * 100 - aggregate_ROW * 4 - holes * 20;
}

void tetris_ai_play(int grid[ROW][COLUMN], Tetromino *current) {
    Move best_move = { .x = current->position.x, .rotation = current->rotation, .score = INT_MIN };

    Tetromino temp;
    int sim[ROW][COLUMN];

    for (int rot = 0; rot < 4; rot++) {
        temp = *current;
        temp.rotation = rot;
        update_blocks(&temp);

        int min_x = COLUMN, max_x = -1;
        for (int i = 0; i < 4; i++) {
            if (temp.blocks[i].x < min_x) min_x = temp.blocks[i].x;
            if (temp.blocks[i].x > max_x) max_x = temp.blocks[i].x;
        }

        for (int x = -min_x; x < COLUMN - max_x; x++) {
            temp = *current;
            temp.rotation = rot;
            temp.position.x = x;
            temp.position.y = 0;
            update_blocks(&temp);

            // Tombe jusqu'à collision
            while (!check_collision(grid, &temp)) {
                temp.position.y++;
                update_blocks(&temp);
            }
            temp.position.y--;
            update_blocks(&temp);

            if (check_collision(grid, &temp)) continue;

            copy_grid(sim, grid);
            apply_tetromino(sim, &temp);
            int score = evaluate_grid(sim);

            if (score > best_move.score) {
                best_move.x = x;
                best_move.rotation = rot;
                best_move.score = score;
            }
        }
    }

    current->rotation = best_move.rotation;
    update_blocks(current);
    current->position.x = best_move.x;

    while (!check_collision(grid, current)) {
        current->position.y++;
        update_blocks(current);
    }
    current->position.y--;
    update_blocks(current);
}
