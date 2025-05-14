#define _POSIX_C_SOURCE 199309L

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "tetris_logic.h"

#define BLOC_SIZE 20
#define WINDOW_WIDTH (NB_BLOCS_W * (BLOC_SIZE) - 1)
#define WINDOW_HEIGHT (NB_BLOCS_H * (BLOC_SIZE) - 1)

TetrominoCollection tetrominos;
TetrominoColorCollection tetrominos_color;
Tetromino currentTetromino;

int grid[NB_BLOCS_H][NB_BLOCS_W] = {0};
int fixedGrid[NB_BLOCS_H][NB_BLOCS_W] = {0};

int startX = 0;
int startY = 3;

Uint64 prev, now; // timers
double delta_t;  // frame duration in milliseconds
bool quit = false;

SDL_Window* window;
SDL_Renderer *renderer;

// Function to initialize the SDL window, SDL renderer and tetromino collections
void init()
{
	window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	tetrominos = initTetrominoCollection();
	tetrominos_color = initTetrominoColorCollection();
}

// Function to initialize the SDL grid
void init_grid(SDL_Renderer *renderer) {
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

// Function to draw the tetrominos on the SDL grid
void draw_tetrominos(SDL_Renderer *renderer)
{
	for (size_t i = 0; i < NB_BLOCS_H; i++)
	{
		for (size_t j = 0; j < NB_BLOCS_W; j++)
		{
			if (grid[i][j] != 0) {
				const TetrominoColor draw_color = getTetrominoColor(grid[i][j]);
				SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
				SDL_Rect bloc_to_draw = { j * BLOC_SIZE, i * BLOC_SIZE, BLOC_SIZE, BLOC_SIZE };
				SDL_RenderFillRect(renderer, &bloc_to_draw);
			}
		}
	}
}

// Function to display the SDL grid with tetrominos
void draw_grid(SDL_Renderer *renderer)
{
	init_grid(renderer);
	draw_tetrominos(renderer);
}

int main(int argc, char** argv)
{
	// Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }
	init();
	init_grid(renderer);
	SDL_RenderPresent(renderer);
	bool quit = false;

    // Initialize tetromino collection
    tetrominos = initTetrominoCollection();
    tetrominos_color = initTetrominoColorCollection();

    // Initialize current tetromino
    currentTetromino = getRandomTetromino(&tetrominos);

    // Check whether the part can be placed at the beginning
    if (!canPlace(currentTetromino, startX, startY, fixedGrid)) {
        // printf("Game Over !\n");
        return 0;
    }

	struct timespec lastUpdate;
	clock_gettime(CLOCK_MONOTONIC, &lastUpdate);

	srand(time(NULL));
	
	while (!quit)
	{
		SDL_Event event;
		while (!quit && SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						// Keyboard events detection
						case SDLK_d:  
							if (canPlace(currentTetromino, startX, startY + 1, fixedGrid)) startY++;
							break;
						case SDLK_q:  
							if (canPlace(currentTetromino, startX, startY - 1, fixedGrid)) startY--;
							break;
						case SDLK_s:  
							if (canPlace(currentTetromino, startX + 1, startY, fixedGrid)) startX++;
							break;
						case SDLK_x:  
							Tetromino temp;
							for (int i = 0; i < 4; i++) {
								for (int j = 0; j < 4; j++) {
									temp.shape[j][3 - i] = currentTetromino.shape[i][j];
								}
							}
								
							if (canPlace(temp, startX, startY, fixedGrid)) {
								for (int i = 0; i < 4; i++) {
									for (int j = 0; j < 4; j++) {
										currentTetromino.shape[i][j] = temp.shape[i][j];
									}
								}
							}
							break;
						case SDLK_ESCAPE: quit = true; break;
						default: break;
					}
					break;
				// case SDL_MOUSEMOTION:	
				//     x_vault += event.motion.xrel;
				//     break;
				// case SDL_MOUSEBUTTONDOWN:
				// 	printf("mouse click %d\n", event.button.button);
				// 	break;
				default: 
					break;
			}
		}

		// Manage the tetromino's descent speed
        struct timespec nowTimer;
        clock_gettime(CLOCK_MONOTONIC, &nowTimer);
        double elapsed = (nowTimer.tv_sec - lastUpdate.tv_sec) + (nowTimer.tv_nsec - lastUpdate.tv_nsec) / 1e9;

        if (elapsed >= 0.5) {
            if (canPlace(currentTetromino, startX + 1, startY, fixedGrid)) {
                startX++;
            } else {
                freezeTetromino(fixedGrid, currentTetromino, startX, startY);
                clearLines(fixedGrid);
                currentTetromino = getRandomTetromino(&tetrominos);
                startX = 0;
                startY = 3;

                // Check if the game is finished
                if (!canPlace(currentTetromino, startX, startY, fixedGrid)) {
                    // displayGrid(grid);
                    // printf("Game Over !\n");
					quit = true;
                    break;
                }
            }

            lastUpdate = nowTimer;
        }

        placeTetromino(grid, fixedGrid, currentTetromino, startX, startY);
        // displayGrid(grid);
        

		prev = now;
		now = SDL_GetPerformanceCounter();
		delta_t = (double)((now - prev) * 1000 / (double)SDL_GetPerformanceFrequency());

		// Updating the window
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    	SDL_RenderClear(renderer);
		draw_grid(renderer);
		SDL_RenderPresent(renderer);
		
		usleep(50 * SLEEP_TIME);
	}

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
