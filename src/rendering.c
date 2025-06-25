#include "rendering.h"

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

void render_menu(SDL_Renderer *renderer, TTF_Font *title_font, TTF_Font *general_font, int selected_item, int window_width) {
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
        TTF_SizeText(general_font, menu_items[i], &text_w, &text_h);
        render_text(renderer, (window_width - text_w) / 2, 200 + i * 50, menu_items[i], general_font, color);
    }
}
