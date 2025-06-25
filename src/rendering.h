#ifndef RENDERING_H
#define RENDERING_H

#include <ncurses.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include <math.h>
#include "AITetrisPlayer.h"
#include "tetromino.h"

#define BLOC_SIZE 20
#define GAME_WINDOW_WIDTH (NB_BLOCS_W * (BLOC_SIZE) - 1)
#define GAME_WINDOW_HEIGHT (NB_BLOCS_H * (BLOC_SIZE) - 1)

void render_text(SDL_Renderer *renderer, int x, int y, const char *text, TTF_Font *font, SDL_Color color);
void render_game_info(SDL_Renderer* renderer, TTF_Font* font, GameState* state, int x_offset);
void render_menu(SDL_Renderer *renderer, TTF_Font *title_font, TTF_Font *general_font, int selected_item, int window_width);

#endif