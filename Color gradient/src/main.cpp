// https://wiki.libsdl.org/SDL_CreateRenderer

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>


// HSL to RGB function taken from https://stackoverflow.com/questions/2353211/hsl-to-rgb-color-conversion

/** Helper method that converts hue to rgb */
	float hueToRgb(float p, float q, float t) {
	    if (t < 0.f)
	        t += 1.f;
	    if (t > 1.f)
	        t -= 1.f;
	    if (t < 1.f/6.f)
	        return p + (q - p) * 6.f * t;
	    if (t < 1.f/2.f)
	        return q;
	    if (t < 2.f/3.f)
	        return p + (q - p) * (2.f/3.f - t) * 6.f;
	    return p;
	}


SDL_Color hslToRgb(float h, float s, float l) {
	    float r, g, b;

	    if (s == 0.f) {
	        r = g = b = l; // achromatic
	    } else {
	        float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
	        float p = 2 * l - q;
	        r = hueToRgb(p, q, h + 1.f/3.f);
	        g = hueToRgb(p, q, h);
	        b = hueToRgb(p, q, h - 1.f/3.f);
	    }
	    SDL_Color rgb;
	    rgb.r = (int) (r * 255);
	    rgb.g = (int) (g * 255);
	    rgb.b = (int) (b * 255);
	    return rgb;
	}


int main(int, char**){

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Window title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Surface* surface = SDL_GetWindowSurface(window);

    SDL_Rect* rect = new SDL_Rect;

	rect->x = 0;
	rect->y = 0;
	rect->w = 500;
	rect->h = 500;

	SDL_Texture* texture;

	SDL_Color color;

	bool running = true;

	SDL_Event event;

	float hue = 0;

	bool direction = true;

	while(running) {
		while( SDL_PollEvent( &event ) ) {
			if( event.type == SDL_QUIT ) {
				running = false;
			}
		}

		if(direction) {
			hue += 0.001;
			if(hue > 1.f) {
				hue = 1;
				direction = !direction;
			}
		}

		else if(!direction) {
			hue -= 0.001;
			if(hue < 0.f) {
			 hue = 0;
			 direction = !direction;
			}
		}

		std::cout << hue << std::endl;

		color = hslToRgb(hue,1,0.5);

		SDL_PixelFormat * pixelformat = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));

		SDL_FillRect(surface,rect,SDL_MapRGB(pixelformat,color.r,color.g,color.b));

		SDL_DestroyTexture(texture);

		texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_FreeSurface(surface);

		SDL_RenderClear(renderer);

		SDL_RenderCopy(renderer, texture, NULL, NULL);

		SDL_RenderPresent(renderer);

		SDL_Delay(1);
	}

	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
