#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "Tetromino.h"

// This file will contain the main function and the game loop for the Tetris, (it may still contain bits of inspired code from an arkanoid game)

#define BLOC_SIZE 20
#define NB_BLOCS_W 10
#define NB_BLOCS_H 21
#define WINDOW_WIDTH (NB_BLOCS_W * (BLOC_SIZE) - 1)
#define WINDOW_HEIGHT (NB_BLOCS_H * (BLOC_SIZE) - 1)

struct coords starting_position = {3 * BLOC_SIZE, 0 * BLOC_SIZE};

int game_board[NB_BLOCS_H][NB_BLOCS_W] = {0};
TetrominoCollection tetrominos;
TetrominoColorCollection tetrominos_color;

Uint64 prev, now; // timers
double delta_t;  // durée frame en ms
bool quit = false;

SDL_Window* window;
SDL_Renderer *renderer;

void init()
{
	window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	tetrominos = initTetrominoCollection();
	tetrominos_color = initTetrominoColorCollection();
}

void init_grid(SDL_Renderer *renderer) {
	printf("Initiating grid visuals\n");

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
	SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White grid borders
    for (int i = 0; i < WINDOW_HEIGHT; i++) {
        SDL_RenderDrawLine(renderer, 0, i * BLOC_SIZE, WINDOW_WIDTH * BLOC_SIZE, i * BLOC_SIZE);
    }
    for (int j = 0; j < WINDOW_WIDTH; j++) {
        SDL_RenderDrawLine(renderer, j * BLOC_SIZE, 0, j * BLOC_SIZE, WINDOW_HEIGHT * BLOC_SIZE);
    }
}

void draw_tetrominos(SDL_Renderer *renderer)
{
	for (size_t i = 0; i < NB_BLOCS_H - 1; i++)
	{
		for (size_t j = 0; j < NB_BLOCS_W - 1; j++)
		{
			if (game_board[i][j] != 0) {
				const TetrominoColor draw_color = getTetrominoColor(game_board[i][j]);
				SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
				SDL_Rect bloc_to_draw = { starting_position.x + j * BLOC_SIZE, starting_position.y + i * BLOC_SIZE, BLOC_SIZE, BLOC_SIZE };
				SDL_RenderFillRect(renderer, &bloc_to_draw);
			}
		}
	}
}

// fonction qui met à jour la surface de la fenetre
void draw_grid(SDL_Renderer *renderer)
{
	printf("Updating grid visuals...\n");

	init_grid(renderer);
	draw_tetrominos(renderer);
}

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

	init();
	init_grid(renderer);
	SDL_RenderPresent(renderer);

	// TEST
	game_board[5][5] = 1;
	game_board[5][6] = 2;
	// FIN TEST
    
	bool quit = false;
	while (!quit)
	{
		SDL_Event event;
		while (!quit && SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			// case SDL_KEYDOWN:
			// 	switch (event.key.keysym.sym)
			// 	{
			// 		// touche clavier
			// 		case SDLK_LEFT:  x_vault -= 10; break;
			// 		case SDLK_RIGHT: x_vault +=10; break;
			// 		case SDLK_ESCAPE: quit = true; break;
			// 		default: break;
			// 	}
			// 	break;
			// case SDL_MOUSEMOTION:	
            //     x_vault += event.motion.xrel;
            //     break;
			// case SDL_MOUSEBUTTONDOWN:
			// 	printf("mouse click %d\n", event.button.button);
				// break;
			default: 
                break;
			}
		}
		prev = now;
		now = SDL_GetPerformanceCounter();
		delta_t = (double)((now - prev) * 1000 / (double)SDL_GetPerformanceFrequency());
		// Use this call to draw_grid(renderer) in order to update the window
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    	SDL_RenderClear(renderer);

		draw_grid(renderer);
		SDL_RenderPresent(renderer);
		// SDL_UpdateWindowSurface(window); 
	}

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
