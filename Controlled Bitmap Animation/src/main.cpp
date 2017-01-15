// https://wiki.libsdl.org/SDL_CreateRenderer

#include <iostream>
#include <SDL.h>
#include <vector>
#include "SDL_image.h"

using std::vector;

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



	vector<SDL_Rect> bullet_vector;

	SDL_Rect enemy = SDL_Rect();
	enemy.w = 50;
	enemy.h = 50;

	int x = 50, y = 500-100;

	SDL_Event event;

	bool running = true;

	bool direction = true;

	bool moving = false;

	bool shooting = false;

	SDL_RenderCopy(renderer, texture, NULL, &actor);

	//SDL_RenderCopy(renderer, texture, NULL, &bullet);

	SDL_RenderCopy(renderer, texture, NULL, &enemy);

	int enemyPos = 250;
	enemy.x = enemyPos;
	enemy.y = 50;

	bool enemyDir = true;

	bool collided = false;

	while(running) {
		SDL_Delay(5);

		SDL_RenderClear(renderer);

		actor.x = x;
		actor.y = y;

		if(shooting == true) {
			SDL_Rect bullet = SDL_Rect();
			bullet.w = 10;
			bullet.h = 10;
			bullet.x = x+50;
			bullet.y = 500-100;

			bullet_vector.push_back(bullet);

			shooting = false;
		}

		SDL_Rect window_borders;
		window_borders.x = 0;
		window_borders.y = 0;
		window_borders.h = 500;
		window_borders.w = 500;

		// http://stackoverflow.com/questions/10957531/c-vector-iterator-erase-last-item-crash
		for (std::vector<SDL_Rect>::iterator it = bullet_vector.begin() ; it != bullet_vector.end(); /* nothing */) {
			SDL_RenderCopy(renderer, texture, NULL, it.base());
			it.base()->y--;

			if(SDL_HasIntersection(&enemy, it.base()) == SDL_TRUE) {
				collided = true;
				std::cout << "Hit!" << std::endl;
			}

			if(SDL_HasIntersection(&window_borders, it.base()) == SDL_FALSE) {
				it = bullet_vector.erase(it);
			} else {
				it++;
			}

			std::cout << bullet_vector.size() << std::endl;
		}

		if(enemy.x > 500-1-50) {
			enemyDir = false;
		}

		if(enemy.x < 0) {
			enemyDir = true;
		}

		if(enemyDir) {
			enemyPos++;
		} else {
			enemyPos--;
		}

		enemy.x = enemyPos;

		SDL_RenderCopy(renderer, texture, NULL, &actor);

		if(!collided)
			SDL_RenderCopy(renderer, texture, NULL, &enemy);

		/*if(SDL_HasIntersection(&enemy, &bullet) == SDL_TRUE) {
			collided = true;
			std::cout << "Hit!" << std::endl;
		}*/

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
