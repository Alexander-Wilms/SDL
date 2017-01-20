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
	SDL_Color color;
	for (int i = 0; i < xsize; i++) {
		for (int j = 0; j < ysize; j++) {
			rect->x = i;
			rect->y = j;
			// greyscale
			// SDL_SetRenderDrawColor(renderer, data[i][j], data[i][j], data[i][j],
				//	255);

			// rainbow
			color = calculateColor(data[i][j]);
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
							255);
			SDL_RenderFillRect(renderer, rect);
		}
	}

	SDL_RenderPresent(renderer);
}

SDL_Color Display::calculateColor(int input) {
	SDL_Color color;
	color.r = 0;
	color.g = 0;
	color.b = 0;

	// red
	if(input < 60 || 300 <= input) {
		color.r = 255;
	} else if(60 <= input && input < 120) {
		color.r = (input-60)*(-255./(120-60))+255;
	} else if(240 <= input && input < 300) {
		color.r = (input-240)*(255./(300-240));
	} else {
		color.r = 0;
	}

	// green
	if(240 <= input) {
		color.g = 0;
	} else if(0 <= input && input < 60) {
		color.g = input * 255./60;
	} else if(180 <= input && input < 240) {
		color.r = (input-180)*(-255./(240-180))+255;
	} else {
		color.g = 255;
	}

	// blue
	if(input < 120) {
		color.b = 0;
	} else if(120 <= input && input < 180) {
		color.b = (input-120)*(255./(180-120));
	} else if(300 <= input) {
		color.b = (input-300)*(-255./(360-300))+255;
	} else {
		color.b = 155;
	}

	return color;
}

Display::~Display() {
	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();
}

