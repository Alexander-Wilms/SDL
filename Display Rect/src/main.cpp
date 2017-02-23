// https://wiki.libsdl.org/SDL_CreateRenderer

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

int main(int, char**){

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Window title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// SDL_Surface* surface = SDL_LoadBMP("test.bmp");

	SDL_Surface* surface = SDL_GetWindowSurface(window);

        SDL_Rect* rect = new SDL_Rect;

	rect->x = 100;
	rect->y = 200;
	rect->w = 300;
	rect->h = 400;

	SDL_FillRect(surface,rect,123);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);

	SDL_Delay(3000);

	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
