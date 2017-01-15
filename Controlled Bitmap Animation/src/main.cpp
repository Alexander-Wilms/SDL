// https://wiki.libsdl.org/SDL_CreateRenderer

#include <iostream>
#include <SDL.h>
#include "SDL_image.h"

int main(int, char**){

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Window title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);

	// SDL_Surface* surface = SDL_LoadBMP("image.bmp");

	SDL_Surface* surface = IMG_Load("image.png");

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	SDL_Rect actor = SDL_Rect();
	actor.w = 100;
	actor.h = 100;

	SDL_Rect bullet = SDL_Rect();
	bullet.w = 10;
	bullet.h = 10;




	int x = 50, y = 50;

	SDL_Event event;

	bool running = true;

	bool direction = true;

	bool moving = false;

	bool shooting = false;

	SDL_RenderCopy(renderer, texture, NULL, &actor);

	SDL_RenderCopy(renderer, texture, NULL, &bullet);

	while(running) {
		SDL_Delay(5);

		SDL_RenderClear(renderer);

		actor.x = x;
		actor.y = y;

		if(shooting == true) {
			shooting = false;
			bullet.x = x+50;
			bullet.y = 50;
		}

		bullet.y--;

		SDL_RenderCopy(renderer, texture, NULL, &actor);

		SDL_RenderCopy(renderer, texture, NULL, &bullet);

		//SDL_RenderDrawRect(renderer, &bullet);

		SDL_RenderPresent(renderer);

		if(moving) {
			if(direction) {
				x++;
			} else {
				x--;
			}
		}

		while( SDL_PollEvent( &event ) ) {
			if( event.type == SDL_QUIT ) {
				SDL_DestroyTexture(texture);

					SDL_DestroyRenderer(renderer);

					SDL_DestroyWindow(window);

					SDL_Quit();

					running = false;
			} else if(event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
				case SDLK_RIGHT:
					moving = true;
					direction = true;
					break;
				case SDLK_LEFT:
					moving = true;
					direction = false;
					break;
				case SDLK_SPACE:
					shooting = true;
				}
			} else if(event.type == SDL_KEYUP) {
				switch(event.key.keysym.sym) {
				case SDLK_RIGHT:
					moving = false;
					break;
				case SDLK_LEFT:
					moving = false;
					break;
				case SDLK_SPACE:
					shooting = false;
				}
			}
		}
	}

	return 0;
}
