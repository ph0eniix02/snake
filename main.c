#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#define SDL_MAIN_USE_CALLBACKS
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>
#include "state.h"

static double old_time = 0;
static double movement_timer = 0;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	App *app = SDL_calloc(1, sizeof(App));
	*appstate = app;
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		return SDL_APP_FAILURE;
	}
	if (!SDL_CreateWindowAndRenderer("Snake", 960, 540, SDL_WINDOW_RESIZABLE, &app->window, &app->renderer))
	{
		return SDL_APP_FAILURE;
	}
	SDL_SetRenderLogicalPresentation(app->renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

	app->board_rect = (SDL_FRect) {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	app->snake_length = 1;
	app->snake_dir = EAST;
	app->snake[0] = N_TILES_WIDTH * N_TILES_HEIGHT / 2 + N_TILES_WIDTH / 2 - 1;
	app->apple = app->snake[0] + 5;
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
	
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	App *app = appstate;
	const bool *key_states = SDL_GetKeyboardState(NULL);

	/* Logic */
	movement_timer += SDL_GetTicks() / (double)SDL_MS_PER_SECOND - old_time;
	old_time = SDL_GetTicks() / (float)SDL_MS_PER_SECOND;

	// printf("%d\n", (app->snake[0] + 1) % N_TILES_WIDTH);
	if (key_states[SDL_SCANCODE_RIGHT])
	{
		if (app->snake_dir != WEST) app->snake_dir = EAST;
	}
	if (key_states[SDL_SCANCODE_LEFT])
	{
		if (app->snake_dir != EAST) app->snake_dir = WEST;
	}
	if (key_states[SDL_SCANCODE_UP])
	{
		if (app->snake_dir != SOUTH) app->snake_dir = NORTH;
	}
	if (key_states[SDL_SCANCODE_DOWN])
	{
		if (app->snake_dir != NORTH) app->snake_dir = SOUTH;
	}
	if (movement_timer >= 0.25)
	{
		movement_timer = 0;
		app->snake_prev_tail = app->snake[app->snake_length - 1];
		for (int i = app->snake_length - 1; i > 0; i--) {
			app->snake[i] = app->snake[i-1];
		}
		switch (app->snake_dir)
		{
			case NORTH:
				if (app->snake[0] < N_TILES_WIDTH)
				{
					printf("Snake ran into the north wall. Ouch\n");
					return SDL_APP_SUCCESS;
				}
				app->snake[0] -= N_TILES_WIDTH;
				break;
			case EAST:
				if ((app->snake[0] + 1) % N_TILES_WIDTH == 0)
				{
					printf("Snake ran into the east wall. Ouch\n");
					return SDL_APP_SUCCESS;
				}
				app->snake[0]++;
				break;
			case SOUTH:
				if (app->snake[0] > N_TILES_WIDTH * N_TILES_HEIGHT - N_TILES_WIDTH - 1)
				{
					printf("Snake ran into the south wall. Ouch\n");
					return SDL_APP_SUCCESS;
				}
				app->snake[0] += N_TILES_WIDTH;
				break;
			case WEST:
				if ((app->snake[0] + 1) % N_TILES_WIDTH == 1)
				{
					printf("Snake ran into the west wall. Ouch\n");
					return SDL_APP_SUCCESS;
				}
				app->snake[0]--;
				break;
		}
		for (int i = 1; i < app->snake_length - 1; i++)
		{
			if (app->snake[0] == app->snake[i])
			{
				printf("The snake ran into it's own body. Ouch\n");
				return SDL_APP_SUCCESS;
			}
		}
		if (app->snake[0] == app->apple)
		{
			app->snake_length++;
			app->snake[app->snake_length - 1] = app->snake_prev_tail;
			int tmp = app->apple;
			here:
				for (int i = 0; i < app->snake_length - 1; i++)
				{
					app->apple = SDL_rand(N_TILES_WIDTH * N_TILES_HEIGHT - 1);
					if (app->apple == app->snake[i] || app->apple == tmp)
					{
						goto here;
					}
				}
		}

	}

	// printf("Snake:");
	// for (int i = 0; i < app->snake_length; i++)
	// {
	// 	printf("%d,", app->snake[i]);
	// }
	// printf(" Prev tail: %d", app->snake_prev_tail);
	// printf("\n");

	/* Draw game */
	SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(app->renderer);

	SDL_SetRenderDrawColor(app->renderer, 50, 50, 50, SDL_ALPHA_OPAQUE);
	SDL_RenderRect(app->renderer, &app->board_rect);

	for (int i = 0; i < N_TILES_HEIGHT; i++)
	{
		for (int j = 0; j < N_TILES_WIDTH; j++)
		{
			SDL_RenderFillRect(app->renderer, &(SDL_FRect) {j * TILE_SIZE + 1, i * TILE_SIZE + 3, TILE_SIZE - 2, TILE_SIZE - 2});
		}
	}

	SDL_SetRenderDrawColor(app->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	for (int i = 0; i < app->snake_length; i++)
	{
			SDL_RenderFillRect(app->renderer, &(SDL_FRect) {app->snake[i] % N_TILES_WIDTH * TILE_SIZE + 1, floor(app->snake[i] / (float)N_TILES_WIDTH) * TILE_SIZE + 3, TILE_SIZE - 2, TILE_SIZE - 2});
	}

	SDL_SetRenderDrawColor(app->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(app->renderer, &(SDL_FRect) {app->apple % N_TILES_WIDTH * TILE_SIZE + 1, floor(app->apple / (float)N_TILES_WIDTH) * TILE_SIZE + 3, TILE_SIZE - 2, TILE_SIZE - 2});

	SDL_RenderPresent(app->renderer);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	SDL_free(appstate);
}
