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

const int MYBLA = 500;

Display::Display(int input) {
	rect = new SDL_Rect();

	vector<vector<bool>> data(MYBLA, vector<bool>(MYBLA));

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Window title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MYBLA, MYBLA, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_SetRenderDrawColor(renderer,255,255,255,255);

	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer,0,0,0,255);

	rect->w = 1;
	rect->h = 1;
}

void Display::setdata(vector<vector<bool>> array) {
	data = array;
}

void Display::updateGraphics() {
	SDL_SetRenderDrawColor(renderer,0,0,0,255);

	SDL_RenderClear(renderer);

	srand (time(NULL)+rand());

	SDL_SetRenderDrawColor(renderer,rand() % 255,rand() % 255,rand() % 255,255);

	for(int i = 0; i <MYBLA; i++) {
		for(int j = 0; j <MYBLA; j++) {
			if(data[i][j] == true) {

				rect->x = i;
				rect->y = j;

				SDL_RenderFillRect(renderer, rect);}
		}
	}

	SDL_RenderPresent(renderer);
}

Display::~Display() {
	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();
}

