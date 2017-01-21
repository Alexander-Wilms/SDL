// https://wiki.libsdl.org/SDL_CreateRenderer

#include <iostream>
#include <fstream>
#include <SDL.h>
#include "SDL_image.h"
#include <vector>
#include <iomanip>

using std::cout;
using std::endl;
using std::vector;

void init();

SDL_Window* window;
SDL_Renderer* renderer;

// http://sdl.beuc.net/sdl.wiki/Pixel_Access
vector<int>* getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8* p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

    vector<int>* color = new vector<int>();

    color->push_back(p[0]);
    color->push_back(p[1]);
    color->push_back(p[2]);

    return color;
}

int main(int, char**) {

	SDL_Surface* surface = SDL_LoadBMP("image.bmp");

	vector<vector<int> > bmp_array;
	vector<int>* color;

	for(int x = 0; x < surface->w; x++) {
		for(int y = 0; y < surface->h; y++) {
			color = getpixel(surface, x, y);
			cout << std::setw(3) << color->at(0) << "," << std::setw(3) << color->at(1) << ","  << std::setw(3) << color->at(2) << "\t";
		}
		cout << endl;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);

	SDL_Delay(5000);

	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}

void init() {
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("2D Pathfinding", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, 10, 10, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}
