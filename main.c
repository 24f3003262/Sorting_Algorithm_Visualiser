#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "algorithms.h" // Ensure this is included

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ARRAY_SIZE 100

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    if (!font) return;
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_array(SDL_Renderer* renderer, int arr[], SortState *state) {
    int bar_width = SCREEN_WIDTH / ARRAY_SIZE;

    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (i <= state->complete_sweep) {
            SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255); // Green for completed
        } 
        else if (i == state->i) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // Cyan
        } 
        else if (i == state->j) {
            SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255); // Red
        } 
        else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for unsorted
        }
        
        SDL_Rect rect = {
            i * bar_width, 
            SCREEN_HEIGHT - arr[i], 
            bar_width - 1, 
            arr[i] 
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Sorting Visualiser", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
    }
    
    int arr[ARRAY_SIZE];
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = rand() % (SCREEN_HEIGHT - 100) + 10;
    }

    // Initialize State and Engine variables
    SortState state;

    AlgoType current_algo = INSERTION;
    reset_sort_state(&state, arr, ARRAY_SIZE,current_algo);

    int running = 1;
    SDL_Event event;
    Uint64 perf_freq = SDL_GetPerformanceFrequency();
    char time_text[128] = "Press SPACE to start | 1-6 to change Algorithm";

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_KEYDOWN) {
                // Toggle Sorting
                if (event.key.keysym.sym == SDLK_SPACE) {
                    state.is_sorting = !state.is_sorting;
                }
                // Reset Array
                else if (event.key.keysym.sym == SDLK_r) {
                    for (int k = 0; k < ARRAY_SIZE; k++) {
                        arr[k] = rand() % (SCREEN_HEIGHT - 100) + 10;
                    }
                    reset_sort_state(&state, arr, ARRAY_SIZE,current_algo);
                    sprintf(time_text, "Ready: %s", get_algo_name(current_algo));
                }
                // Algorithm Selection (Only allow switching when paused or reset)
                else if (!state.is_sorting && state.complete_sweep == -1) {
                    if (event.key.keysym.sym == SDLK_1) current_algo = BUBBLE;
                    if (event.key.keysym.sym == SDLK_2) current_algo = SELECTION;
                    if (event.key.keysym.sym == SDLK_3) current_algo = INSERTION;
                    if (event.key.keysym.sym == SDLK_4) current_algo = MERGE;
                    if (event.key.keysym.sym == SDLK_5) current_algo = QUICK;
                    if (event.key.keysym.sym == SDLK_6) current_algo = RADIX;
                    
                    // Reset array logic when algorithm changes to ensure clean state
                    reset_sort_state(&state, arr, ARRAY_SIZE,current_algo);
                    sprintf(time_text, "Selected: %s", get_algo_name(current_algo));
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // --- Sorting Engine Step ---
        if (state.is_sorting) {
            sort_step(arr, ARRAY_SIZE, &state, current_algo, perf_freq);
            
            // Only update "live" text if it's actually sorting (not finished)
            if (state.is_sorting) {
                sprintf(time_text, "[%s] CPU Time: %.8fs | Comparisons: %d", 
                        get_algo_name(current_algo), state.actual_cpu_time, state.comparisons);
                SDL_Delay(2);
            }
        } 
        
        // --- Completion Sweep Logic ---
        // If sorting has stopped automatically, trigger the sweep
        if (!state.is_sorting && state.complete_sweep < ARRAY_SIZE && state.actual_cpu_time > 0) {
            if (state.complete_sweep == -1) {
                sprintf(time_text, "FINAL [%s] CPU Time: %.8fs | Comparisons: %d", 
                        get_algo_name(current_algo), state.actual_cpu_time, state.comparisons);
            }
            state.complete_sweep++;
            SDL_Delay(5);
        }

        // --- Render ---
        draw_array(renderer, arr, &state);
        if (font) render_text(renderer, font, time_text, 20, 20);
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}