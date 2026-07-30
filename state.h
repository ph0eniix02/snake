#ifndef STATEH
#define STATEH
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 180

#define TILE_SIZE 8
#define N_TILES_WIDTH (int)(WINDOW_WIDTH / TILE_SIZE)
#define N_TILES_HEIGHT (int)(WINDOW_HEIGHT / TILE_SIZE)

typedef enum {NORTH, EAST, SOUTH, WEST} Direction;
typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_FRect board_rect;
	int snake_length;
	int snake_prev_tail;
	Direction snake_dir;
	int snake[N_TILES_WIDTH * N_TILES_HEIGHT - 1];
	int apple;
} App;
#endif 
