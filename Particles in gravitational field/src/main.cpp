// https://wiki.libsdl.org/SDL_CreateRenderer

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "SDL_image.h"
#include "Particle.h"
#include <SDL_opengl.h>
#include <cstdlib>
#include <time.h>
#include <cmath>

using std::vector;

#define CENTERMASS 100

int main(int, char**) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Window title",
	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN);

	SDL_Rect window_borders;
	window_borders.x = 0;
	window_borders.y = 0;
	window_borders.h = 500;
	window_borders.w = 500;

	SDL_Surface* screen_surface = SDL_GetWindowSurface(window);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
			SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);



	vector<Particle*> particle_vector;

	SDL_Event event;
	bool running = true;

	Uint32 starttime = SDL_GetTicks();

	srand(time(NULL));

	float density = 100;

	bool done = false;

	while (running) {

		if (SDL_GetTicks() - starttime > 100) {
			//done = true;
			starttime = SDL_GetTicks();

			int x = rand()%500;
			int y = rand()%500;

			//x = 0;
			//y = 0;

			int size = rand()%50;
			int mass = 100;//density * pow(size,2);

			float xvel = 50. / (rand()%10);
			if(rand()%2)
				xvel *= -1;

			float yvel = 50. / (rand()%10);
			if(rand()%2)
				yvel *= -1;



			particle_vector.push_back(
					new Particle(renderer, x, y, xvel,
							yvel, screen_surface, mass, CENTERMASS, size));



		}

		for (vector<Particle*>::iterator iter = particle_vector.begin();
				iter != particle_vector.end();
				/* nothing */) {
			(*iter)->render();

			if (SDL_HasIntersection(&window_borders, (*iter)->getRect())
					== SDL_FALSE || (*iter)->isFaded()) {
				iter = particle_vector.erase(iter);
			} else {
				iter++;
			}
		}

		SDL_RenderPresent(renderer);

		SDL_RenderClear(renderer);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {

				SDL_DestroyRenderer(renderer);

				SDL_DestroyWindow(window);

				SDL_Quit();

				running = false;
			}
		}
	}

	return 0;
}
