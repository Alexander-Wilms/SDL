// https://wiki.libsdl.org/SDL_CreateRenderer

#include <iostream>
#include <fstream>
#include <SDL.h>
#include "SDL_image.h"
#include <vector>

using std::cout;
using std::endl;
using std::vector;

void init();

SDL_Window* window;
SDL_Renderer* renderer;

// http://sdl.beuc.net/sdl.wiki/Pixel_Access
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

int main(int, char**) {

	SDL_Surface* surface = SDL_LoadBMP("image.bmp");

	vector<vector<int> > bmp_array;
	Uint32 this_pixel;

	for(int x = 0; x < surface->w; x++) {
		for(int y = 0; y < surface->h; y++) {
			this_pixel = getpixel(surface, x, y);
			cout << this_pixel << "\t";
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
