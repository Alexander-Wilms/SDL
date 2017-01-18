#include <SDL.h>
#include <SDL_image.h>
#include "Particle.h"
#include <iostream>
#include <cmath>

Particle::Particle(SDL_Renderer* renderer, int x, int y, double x_velocity,
		double y_velocity, SDL_Surface* screen_surface, float ownMass, float CenterMass, int size) {
	this->renderer = renderer;
	this->screen_surface = screen_surface;
	png_surface = IMG_Load("particle.png");
	surface = SDL_ConvertSurfaceFormat(png_surface, SDL_PIXELFORMAT_ARGB8888,
			0);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	rect.x = x;
	realx = rect.x;
	rect.y = y;
	realy = rect.y;
	rect.w = 20;
	rect.h = 20;
	this->x_velocity = x_velocity;
	this->y_velocity = y_velocity;
	timestamp_creation = SDL_GetTicks();
	alpha = 255;
	faded = false;
}

Particle::~Particle() {
	SDL_DestroyTexture(texture);
}

void Particle::render() {

	float r = sqrt( pow(realx,2)+pow(realy,2));

	float delta_t = SDL_GetTicks() - timestamp_creation;
	realx = realx + delta_t / 1000 * x_velocity;
	realy = realy + delta_t / 1000 * y_velocity;



	rect.x = (int) realx;
	rect.y = (int) realy;

	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

bool Particle::isFaded() {
	return faded;
}

SDL_Rect* Particle::getRect() {
	return &rect;
}
