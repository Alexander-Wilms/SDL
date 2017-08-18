#include <SDL.h>
#include <SDL_image.h>
#include "Particle.h"
#include <iostream>
#include <cmath>

#define PI 3.1416

using std::cout;
using std::endl;

Particle::Particle(SDL_Renderer* renderer, int x, int y, double x_velocity,
		double y_velocity, SDL_Surface* screen_surface, double ownMass, double CenterMass, int size) {
	this->renderer = renderer;
	this->screen_surface = screen_surface;
	png_surface = IMG_Load("particle.png");
	surface = SDL_ConvertSurfaceFormat(png_surface, SDL_PIXELFORMAT_ARGB8888,
			0);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	rect.x = x;
	rect.y = y;
	rect.w = size;
	rect.h = size;
	this->x_velocity = 1;
	this->y_velocity = 1;
	timestamp_creation = SDL_GetTicks();
	alpha = 255;
	faded = false;
	this->CenterMass = CenterMass;
	this->ownMass = ownMass;
	// https://en.wikipedia.org/wiki/Gravitational_constant
	G = 5;
	ay = 0;
	ax = 0;
	angle_of_acting_force = 0;
	a = 0;
	F = 0;
	r = 0;
}

Particle::~Particle() {
	SDL_DestroyTexture(texture);
}

void Particle::render() {
	double delta_t = (SDL_GetTicks() - timestamp_creation) / 1000.;

	// calculate particle's distance from the black hole
	r = sqrt( pow(rect.x-250,2)+pow(rect.y-250,2));

	// calculate gravitational force acting upon the particle
	F = G*(ownMass*CenterMass)/r;

	// calculate resulting acceleration of particle in the direction of the black hole
	a = F / ownMass;

	// calculate the direction in which the acceleration occurs
	cout << "x: " << rect.x << endl;
	cout << "y: " << rect.y << endl;
	angle_of_acting_force = atan2(-(rect.y-250),rect.x-250);
	angle_of_acting_force = (angle_of_acting_force + PI);
	cout << "angle of acting force (in deg): " << angle_of_acting_force *180/3.1415<< endl;

	// splitting up the accelertion into two components
	ax = a*cos(angle_of_acting_force);
	ay = -a*sin(angle_of_acting_force);

	// calculating the new velocity of the particle
	// v_new = a*t+v
	x_velocity = ax * delta_t + x_velocity;
	y_velocity = ay * delta_t + y_velocity;

	// calculating the new position of the particle
	// x_new = 0.5*a*t^2+v*t+x
	rect.x = 0.5 * ax * pow(delta_t, 2) + delta_t * x_velocity + rect.x;
	rect.y = 0.5 * ay * pow(delta_t, 2) + delta_t * y_velocity + rect.y;

	SDL_RenderCopy(renderer, texture, NULL, &rect);

	SDL_RenderPresent(renderer);
}

bool Particle::isFaded() {
	return faded;
}

SDL_Rect* Particle::getRect() {
	return &rect;
}

double Particle::SafeAcos (double x) {
  if (x < -1.0)
	  x = -1.0 ;
  else if (x > 1.0)
	  x = 1.0 ;

  return acos (x) ;
}
