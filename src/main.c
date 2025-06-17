#define _POSIX_C_SOURCE 199309L

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include "tetris_logic.h"

#define BLOC_SIZE 20
#define GAME_WINDOW_WIDTH (NB_BLOCS_W * (BLOC_SIZE) - 1)
#define GAME_WINDOW_HEIGHT (NB_BLOCS_H * (BLOC_SIZE) - 1)

TetrominoColorCollection tetrominos_color;

SDL_Window* window;
SDL_Renderer *renderer;
TTF_Font *title_font;
TTF_Font *menu_font;

// Function to initialize the SDL window and renderer
void init(int width, int height)
{
	window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
}

// Function to initialize the SDL grid
void init_grid(SDL_Renderer *renderer, int x_offset) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White grid lines
    int grid_pixel_width = NB_BLOCS_W * BLOC_SIZE;
    int grid_pixel_height = NB_BLOCS_H * BLOC_SIZE;

    // Draw horizontal lines
    for (int i = 0; i <= NB_BLOCS_H; i++) {
        SDL_RenderDrawLine(renderer, x_offset, i * BLOC_SIZE, x_offset + grid_pixel_width, i * BLOC_SIZE);
    }
    // Draw vertical lines
    for (int j = 0; j <= NB_BLOCS_W; j++) {
        SDL_RenderDrawLine(renderer, x_offset + j * BLOC_SIZE, 0, x_offset + j * BLOC_SIZE, grid_pixel_height);
    }
}

// Function to draw the tetrominos on the SDL grid
void draw_tetrominos(SDL_Renderer *renderer, int grid[NB_BLOCS_H][NB_BLOCS_W], int x_offset)
{
	for (size_t i = 0; i < NB_BLOCS_H; i++)
	{
		for (size_t j = 0; j < NB_BLOCS_W; j++)
		{
			if (grid[i][j] != 0) {
				const TetrominoColor draw_color = getTetrominoColor(grid[i][j]);
				SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
				SDL_Rect bloc_to_draw = { x_offset + j * BLOC_SIZE, i * BLOC_SIZE, BLOC_SIZE, BLOC_SIZE };
				SDL_RenderFillRect(renderer, &bloc_to_draw);
			}
		}
	}
}

// Function to display the SDL grid with tetrominos
void draw_grid(SDL_Renderer *renderer, int grid[NB_BLOCS_H][NB_BLOCS_W], int x_offset)
{
	init_grid(renderer, x_offset);
	draw_tetrominos(renderer, grid, x_offset);
}

void render_text(SDL_Renderer *renderer, int x, int y, const char *text, TTF_Font *font, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        printf("Failed to create surface from text: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Failed to create texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void render_game_info(SDL_Renderer* renderer, TTF_Font* font, GameState* state, int x_offset) {
    SDL_Color white = {255, 255, 255, 255};
    char text_buffer[50];
    int text_w, text_h;

    // Render Level on the left
    sprintf(text_buffer, "Level: %d", state->level);
    TTF_SizeText(font, text_buffer, &text_w, &text_h);
    render_text(renderer, (x_offset - text_w) / 2, GAME_WINDOW_HEIGHT / 2 - text_h, text_buffer, font, white);

    // Render Lines for next level on the left (under level)
    sprintf(text_buffer, "Lines: %d/10", state->lines_cleared_for_level);
    TTF_SizeText(font, text_buffer, &text_w, &text_h);
    render_text(renderer, (x_offset - text_w) / 2, GAME_WINDOW_HEIGHT / 2 + 5, text_buffer, font, white);

    // Render Total Lines on the right
    sprintf(text_buffer, "Total : %d", state->total_lines_cleared);
    TTF_SizeText(font, text_buffer, &text_w, &text_h);
    render_text(renderer, x_offset + (NB_BLOCS_W * BLOC_SIZE) + (x_offset - text_w) / 2, GAME_WINDOW_HEIGHT / 2 - text_h, text_buffer, font, white);
}

void render_menu(SDL_Renderer *renderer, TTF_Font *title_font, TTF_Font *menu_font, int selected_item, int window_width) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    // Render Title
    int text_w, text_h;
    const char* title_text = "Tetris";
    TTF_SizeText(title_font, title_text, &text_w, &text_h);
    render_text(renderer, (window_width - text_w) / 2, GAME_WINDOW_HEIGHT / 4, title_text, title_font, white);

    // Render Menu Items
    const char* menu_items[] = {"Single Player", "Player vs AI (Not available)", "Exit"};
    int y_pos = GAME_WINDOW_HEIGHT / 2;

    for (int i = 0; i < 3; i++) {
        TTF_SizeText(menu_font, menu_items[i], &text_w, &text_h);
        render_text(renderer, (window_width - text_w) / 2, y_pos, menu_items[i], menu_font, i == selected_item ? yellow : white);
        y_pos += 40;
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char** argv)
{
	// Initialize SDL
        bool quit = false;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(TTF_Init() == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize TTF: %s", TTF_GetError());
        return EXIT_FAILURE;
    }

    title_font = TTF_OpenFont("../assets/font.ttf", 28);
    if (title_font == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load title font: %s", TTF_GetError());
        return EXIT_FAILURE;
    }

    menu_font = TTF_OpenFont("../assets/font.ttf", 18);
    if (menu_font == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load menu font: %s", TTF_GetError());
        TTF_CloseFont(title_font);
        return EXIT_FAILURE;
    }

    int max_w = 0;
    int text_w;

    TTF_SizeText(title_font, "Tetris", &text_w, NULL);
    if (text_w > max_w) {
        max_w = text_w;
    }

    const char* menu_texts[] = {"Single Player", "Player vs AI (Not available)", "Exit"};
    for (int i=0; i<3; ++i) {
        TTF_SizeText(menu_font, menu_texts[i], &text_w, NULL);
        if (text_w > max_w) {
            max_w = text_w;
        }
    }

    int menu_width = max_w + 100; // Add some padding
    int final_width = (menu_width > GAME_WINDOW_WIDTH) ? menu_width : GAME_WINDOW_WIDTH;

	init(final_width, GAME_WINDOW_HEIGHT);

    bool in_menu = true;
    int selected_item = 0;
    const int menu_items_count = 3;

    while (in_menu) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                in_menu = false;
                quit = true;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        selected_item = (selected_item - 1 + menu_items_count) % menu_items_count;
                        break;
                    case SDLK_DOWN:
                        selected_item = (selected_item + 1) % menu_items_count;
                        break;
                    case SDLK_RETURN:
                        switch (selected_item) {
                            case 0: // Single Player
                                in_menu = false;
                                break;
                            case 1: // Player vs AI
                                // Do nothing for now
                                break;
                            case 2: // Exit
                                in_menu = false;
                                quit = true;
                                break;
                        }
                        break;
                    case SDLK_ESCAPE:
                        in_menu = false;
                        quit = true;
                        break;
                }
            }
        }
        render_menu(renderer, title_font, menu_font, selected_item, final_width);
        SDL_Delay(10);
    }

    if (quit) {
        TTF_CloseFont(title_font);
        TTF_CloseFont(menu_font);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    int game_grid_width = NB_BLOCS_W * BLOC_SIZE;
    int x_offset = (final_width - game_grid_width) / 2;

    int grid[NB_BLOCS_H][NB_BLOCS_W] = {0};
    int fixedGrid[NB_BLOCS_H][NB_BLOCS_W] = {0};
    GameState game_state;
    init_game_state(&game_state);

    int startX = 0;
    int startY = 3;

    Uint64 prev, now; // timers
    now = SDL_GetPerformanceCounter();
    double delta_t;  // frame duration in milliseconds

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
	init_grid(renderer, x_offset);
	SDL_RenderPresent(renderer);

    // Initialize tetromino collection
    TetrominoCollection tetrominos = initTetrominoCollection();
    tetrominos_color = initTetrominoColorCollection();

    // Initialize current tetromino
    Tetromino currentTetromino = getRandomTetromino(&tetrominos);

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
                int cleared_lines = clearLines(fixedGrid);
                update_game_state(&game_state, cleared_lines);
                currentTetromino = getRandomTetromino(&tetrominos);
                startX = 0;
                startY = 3;

                // Check if the game is finished
                if (!canPlace(currentTetromino, startX, startY, fixedGrid)) {
                    quit = true;
                    break;
                }
            }

            lastUpdate = nowTimer;
        }

        placeTetromino(grid, fixedGrid, currentTetromino, startX, startY);
        
		prev = now;
		now = SDL_GetPerformanceCounter();
		delta_t = (double)((now - prev) * 1000 / (double)SDL_GetPerformanceFrequency());

		// Updating the window
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    	SDL_RenderClear(renderer);
		draw_grid(renderer, grid, x_offset);
        render_game_info(renderer, menu_font, &game_state, x_offset);
		SDL_RenderPresent(renderer);
		
		usleep(50 * SLEEP_TIME);
	}

    TTF_CloseFont(title_font);
    TTF_CloseFont(menu_font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
