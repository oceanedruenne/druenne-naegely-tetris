#define _POSIX_C_SOURCE 199309L

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include <math.h>
#include "tetris_logic.h"
#include "tetromino.h"
#include "AITetrisPlayer.h"

#define BLOC_SIZE 20
#define GAME_WINDOW_WIDTH (NB_BLOCS_W * (BLOC_SIZE) - 1)
#define GAME_WINDOW_HEIGHT (NB_BLOCS_H * (BLOC_SIZE) - 1)

TetrominoColorCollection tetrominos_color;

SDL_Window* window;
SDL_Renderer *renderer;
TTF_Font *title_font;
TTF_Font *menu_font;

void init(int width, int height)
{
    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
}

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
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    // Render title
    int text_w, text_h;
    TTF_SizeText(title_font, "Tetris", &text_w, &text_h);
    render_text(renderer, (window_width - text_w) / 2, 50, "Tetris", title_font, white);

    // Render menu items
    const char *menu_items[] = {"Single Player", "Player vs AI", "Exit"};
    for (int i = 0; i < 3; i++) {
        SDL_Color color = (i == selected_item) ? yellow : white;
        TTF_SizeText(menu_font, menu_items[i], &text_w, &text_h);
        render_text(renderer, (window_width - text_w) / 2, 200 + i * 50, menu_items[i], menu_font, color);
    }
}

void run_player_vs_ai_game() {
    bool quit = false;
    int player_x_offset = 50;
    int ai_x_offset = (NB_BLOCS_W * BLOC_SIZE) + 100;

    // Player state
    int player_grid[NB_BLOCS_H][NB_BLOCS_W] = {0};
    int player_fixedGrid[NB_BLOCS_H][NB_BLOCS_W] = {0};
    int player_startX = 0;
    int player_startY = 3;
    struct timespec player_lastUpdate;
    clock_gettime(CLOCK_MONOTONIC, &player_lastUpdate);

    // AI state
    int ai_grid[NB_BLOCS_H][NB_BLOCS_W] = {0};
    int ai_fixedGrid[NB_BLOCS_H][NB_BLOCS_W] = {0};
    int ai_startX = 0;
    int ai_startY = 3;
    struct timespec ai_lastUpdate;
    clock_gettime(CLOCK_MONOTONIC, &ai_lastUpdate);

    TetrominoCollection tetrominos = initTetrominoCollection();
    Tetromino player_currentTetromino = getRandomTetromino(&tetrominos);
    Tetromino ai_currentTetromino = getRandomTetromino(&tetrominos);

    if (!canPlace(player_currentTetromino, player_startX, player_startY, player_fixedGrid) ||
        !canPlace(ai_currentTetromino, ai_startX, ai_startY, ai_fixedGrid)) {
        return; // Initial game over check
    }

    while (!quit) {
        // --- EVENT HANDLING (PLAYER) ---
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                    case SDLK_LEFT:
                        if (canPlace(player_currentTetromino, player_startX, player_startY - 1, player_fixedGrid)) player_startY--;
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        if (canPlace(player_currentTetromino, player_startX, player_startY + 1, player_fixedGrid)) player_startY++;
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        if (canPlace(player_currentTetromino, player_startX + 1, player_startY, player_fixedGrid)) player_startX++;
                        break;
                    case SDLK_x:
                    case SDLK_UP:
                        try_rotate_tetromino(&player_currentTetromino, &player_startX, &player_startY, player_fixedGrid);
                        break;
                    case SDLK_SPACE:
                        while (canPlace(player_currentTetromino, player_startX + 1, player_startY, player_fixedGrid)) {
                            player_startX++;
                        }
                        freezeTetromino(player_fixedGrid, player_currentTetromino, player_startX, player_startY);
                        int cleared_lines = clearLines(player_fixedGrid);
                        if (cleared_lines > 1) {
                            int garbage_to_send = (cleared_lines == 4) ? 4 : cleared_lines - 1;
                            add_garbage_lines(ai_fixedGrid, garbage_to_send);
                            if (!canPlace(ai_currentTetromino, ai_startX, ai_startY, ai_fixedGrid)) {
                                quit = true;
                            }
                        }
                        player_currentTetromino = getRandomTetromino(&tetrominos);
                        player_startX = 0;
                        player_startY = 3;
                        if (!canPlace(player_currentTetromino, player_startX, player_startY, player_fixedGrid)) {
                            quit = true;
                        }
                        clock_gettime(CLOCK_MONOTONIC, &player_lastUpdate);
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                }
            }
        }

        // --- PLAYER GAME LOGIC ---
        struct timespec player_nowTimer;
        clock_gettime(CLOCK_MONOTONIC, &player_nowTimer);
        double player_elapsed = (player_nowTimer.tv_sec - player_lastUpdate.tv_sec) + (player_nowTimer.tv_nsec - player_lastUpdate.tv_nsec) / 1e9;

        if (player_elapsed >= 0.5) { // Automatic fall
            if (canPlace(player_currentTetromino, player_startX + 1, player_startY, player_fixedGrid)) {
                player_startX++;
            } else {
                freezeTetromino(player_fixedGrid, player_currentTetromino, player_startX, player_startY);
                int cleared_lines = clearLines(player_fixedGrid);
                if (cleared_lines > 1) { // Send garbage for 2+ lines
                    int garbage_to_send = (cleared_lines == 4) ? 4 : cleared_lines - 1;
                    add_garbage_lines(ai_fixedGrid, garbage_to_send);
                    if (!canPlace(ai_currentTetromino, ai_startX, ai_startY, ai_fixedGrid)) {
                        quit = true; // AI loses due to garbage
                    }
                }
                player_currentTetromino = getRandomTetromino(&tetrominos);
                player_startX = 0;
                player_startY = 3;
                if (!canPlace(player_currentTetromino, player_startX, player_startY, player_fixedGrid)) {
                    quit = true; // Player loses
                }
            }
            player_lastUpdate = player_nowTimer;
        }

        // --- AI GAME LOGIC ---
        struct timespec ai_nowTimer;
        clock_gettime(CLOCK_MONOTONIC, &ai_nowTimer);
        double ai_elapsed = (ai_nowTimer.tv_sec - ai_lastUpdate.tv_sec) + (ai_nowTimer.tv_nsec - ai_lastUpdate.tv_nsec) / 1e9;
        
        if (ai_elapsed >= 0.01) { // AI plays faster
            char move = tetris_ai_play(ai_grid, ai_fixedGrid, ai_currentTetromino, &ai_startX, &ai_startY);
            if (move == 'd') { // Right
                if (canPlace(ai_currentTetromino, ai_startX, ai_startY + 1, ai_fixedGrid)) ai_startY++;
            } else if (move == 'q') { // Left
                if (canPlace(ai_currentTetromino, ai_startX, ai_startY - 1, ai_fixedGrid)) ai_startY--;
            } else if (move == 'x') { // Rotate
                Tetromino temp = ai_currentTetromino;
                rotate_tetromino(&temp);
                if (canPlace(temp, ai_startX, ai_startY, ai_fixedGrid)) ai_currentTetromino = temp;
            } else if (move == 's') { // Down
                if (canPlace(ai_currentTetromino, ai_startX + 1, ai_startY, ai_fixedGrid)) {
                    ai_startX++;
                } else {
                    freezeTetromino(ai_fixedGrid, ai_currentTetromino, ai_startX, ai_startY);
                    int cleared_lines = clearLines(ai_fixedGrid);
                    if (cleared_lines > 1) { // Send garbage for 2+ lines
                        int garbage_to_send = (cleared_lines == 4) ? 4 : cleared_lines - 1;
                        add_garbage_lines(player_fixedGrid, garbage_to_send);
                        if (!canPlace(player_currentTetromino, player_startX, player_startY, player_fixedGrid)) {
                            quit = true; // Player loses due to garbage
                        }
                    }
                    ai_currentTetromino = getRandomTetromino(&tetrominos);
                    ai_startX = 0;
                    ai_startY = 3;
                    if (!canPlace(ai_currentTetromino, ai_startX, ai_startY, ai_fixedGrid)) {
                        quit = true; // AI loses
                    }
                }
            }
            ai_lastUpdate = ai_nowTimer;
        }

        // --- RENDERING ---
        placeTetromino(player_grid, player_fixedGrid, player_currentTetromino, player_startX, player_startY);
        placeTetromino(ai_grid, ai_fixedGrid, ai_currentTetromino, ai_startX, ai_startY);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        draw_grid(renderer, player_grid, player_x_offset);
        draw_grid(renderer, ai_grid, ai_x_offset);
        
        SDL_RenderPresent(renderer);
    }
}

void run_single_player_game(int x_offset) {
    bool quit = false;
    int grid[NB_BLOCS_H][NB_BLOCS_W] = {0};
    int fixedGrid[NB_BLOCS_H][NB_BLOCS_W] = {0};
    GameState game_state;
    init_game_state(&game_state); // INITIALISE STATS

    // Current tetromino position
    int X = 0;
    int Y = 3;

    struct timespec lastUpdate;
    clock_gettime(CLOCK_MONOTONIC, &lastUpdate);

    TetrominoCollection tetrominos = initTetrominoCollection();
    Tetromino currentTetromino = getRandomTetromino(&tetrominos);

    if (!canPlace(currentTetromino, X, Y, fixedGrid)) {
        return;
    }

    double fall_speed = 0.5; // Initial fall speed in seconds

    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN) {
                Tetromino temp = currentTetromino;
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                    case SDLK_LEFT:
                        if (canPlace(currentTetromino, X, Y - 1, fixedGrid)) Y--;
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        if (canPlace(currentTetromino, X, Y + 1, fixedGrid)) Y++;
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        if (canPlace(currentTetromino, X + 1, Y, fixedGrid)) Y++;
                        break;
                    case SDLK_x:
                    case SDLK_UP:
                        try_rotate_tetromino(&currentTetromino, &X, &Y, fixedGrid);
                        break;
                    case SDLK_SPACE:
                        while (canPlace(currentTetromino, X + 1, Y, fixedGrid)) {
                            X++;
                        }
                        freezeTetromino(fixedGrid, currentTetromino, X, Y);
                        int cleared_lines = clearLines(fixedGrid);
                        update_game_state(&game_state, cleared_lines);
                        currentTetromino = getRandomTetromino(&tetrominos);
                        X = 0;
                        Y = 3;
                        if (!canPlace(currentTetromino, X, Y, fixedGrid)) {
                            quit = true;
                        }
                        clock_gettime(CLOCK_MONOTONIC, &lastUpdate); // Reset fall timer
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                }
            }
        }

        struct timespec nowTimer;
        clock_gettime(CLOCK_MONOTONIC, &nowTimer);
        double elapsed = (nowTimer.tv_sec - lastUpdate.tv_sec) + (nowTimer.tv_nsec - lastUpdate.tv_nsec) / 1e9;

        if (elapsed >= fall_speed) {
            if (canPlace(currentTetromino, X + 1, Y, fixedGrid)) {
                X++;
            } else {
                freezeTetromino(fixedGrid, currentTetromino, X, Y);
                int cleared_lines = clearLines(fixedGrid);
                int old_level = game_state.level;
                update_game_state(&game_state, cleared_lines);
                if (game_state.level > old_level) {
                    fall_speed = 0.5 * pow(0.85, game_state.level - 1);
                }
                currentTetromino = getRandomTetromino(&tetrominos);
                X = 0;
                Y = 3;

                if (!canPlace(currentTetromino, X, Y, fixedGrid)) {
                    quit = true;
                }
            }
            lastUpdate = nowTimer;
        }

        placeTetromino(grid, fixedGrid, currentTetromino, X, Y);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        draw_grid(renderer, grid, x_offset);
        render_game_info(renderer, menu_font, &game_state, x_offset);
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialization failed: %s %s", SDL_GetError(), TTF_GetError());
        return EXIT_FAILURE;
    }

    int menu_width = 500;
    int menu_height = 400;
    init(menu_width, menu_height);

    title_font = TTF_OpenFont("../assets/font.ttf", 48);
    menu_font = TTF_OpenFont("../assets/font.ttf", 24);
    if (!title_font || !menu_font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    tetrominos_color = initTetrominoColorCollection();

    int selected_item = 0;
    bool quit_menu = false;
    while (!quit_menu) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) { // MENU CHOICES
            if (event.type == SDL_QUIT) { // EXIT SELECTED
                selected_item = 2;
                quit_menu = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: selected_item = (selected_item - 1 + 3) % 3; break;
                    case SDLK_DOWN: selected_item = (selected_item + 1) % 3; break;
                    case SDLK_RETURN: quit_menu = true; break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        render_menu(renderer, title_font, menu_font, selected_item, menu_width);
        SDL_RenderPresent(renderer);
    }

    // Close menu window and open game window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    if (selected_item != 2) {
        if (selected_item == 0) {
            int game_width = (NB_BLOCS_W * BLOC_SIZE) + 600;
            int game_height = (NB_BLOCS_H * BLOC_SIZE);
            int x_offset = (game_width - (NB_BLOCS_W * BLOC_SIZE)) / 2;
            init(game_width, game_height);
            run_single_player_game(x_offset);
        } else if (selected_item == 1) {
            int pvp_width = (NB_BLOCS_W * BLOC_SIZE) * 2 + 150;
            int pvp_height = (NB_BLOCS_H * BLOC_SIZE);
            init(pvp_width, pvp_height);
            run_player_vs_ai_game();
        }
    }

    // Clean up SDL resources
    TTF_CloseFont(title_font);
    TTF_CloseFont(menu_font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
