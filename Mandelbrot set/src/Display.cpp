/*
 * Display.cpp
 *
 *  Created on: 15.01.2017
 *      Author: Alexander Wilms
 */

#include "Display.h"
#include <SDL.h>
#include <cstdlib>
#include <time.h>
#include <iostream>

using std::cout;
using std::endl;

Display::Display(int XSIZE, int YSIZE) {
	rect = new SDL_Rect();

	vector<vector<double>> data(XSIZE, vector<double>(YSIZE));

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Window title", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, XSIZE, YSIZE, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	rect->w = 1;
	rect->h = 1;

	xsize = XSIZE;
	ysize = YSIZE;
}

void Display::setdata(vector<vector<double>> array) {
	data = array;
}

void Display::updateGraphics() {
	for (int i = 0; i < xsize; i++) {
		for (int j = 0; j < ysize; j++) {
			rect->x = i;
			rect->y = j;
			SDL_SetRenderDrawColor(renderer, data[i][j], data[i][j], data[i][j],
					255);
			SDL_RenderFillRect(renderer, rect);
		}
	}

	SDL_RenderPresent(renderer);
}

Display::~Display() {
	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();
}

