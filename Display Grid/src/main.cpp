// https://wiki.libsdl.org/SDL_CreateRenderer

#include <iostream>
#include <SDL.h>

int main(int, char**){

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Window title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawColor(renderer,255,10,50,120);

	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer,37,72,92,120);

	SDL_Rect* rect = new SDL_Rect;

	rect->w = 1;
	rect->h = 1;

	for(int i = 0; i <500; i++) {
		for(int j = 0; j <500; j++) {
			rect->x = i;
			rect->y = j;

			SDL_RenderFillRect(renderer, rect);
		}
	}

	SDL_RenderPresent(renderer);

	SDL_Delay(3000);

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
