#include <SDL.h>
#include <SDL_image.h>
#include "Particle.h"
#include <iostream>
#include <cmath>

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
	startrealx = rect.x;
	rect.y = y;
	startrealy = rect.y;
	rect.w = 20;
	rect.h = 20;
	this->x_velocity = x_velocity;
	this->y_velocity = y_velocity;
	timestamp_creation = SDL_GetTicks();
	alpha = 255;
	faded = false;
	this->CenterMass = CenterMass;
	this->ownMass = ownMass;
	// https://en.wikipedia.org/wiki/Gravitational_constant
	G = 10;
	ay = 0;
	ax = 0;
	gravalpha = 0;
	a = 0;
	F = 0;
	r = 0;
	realy = startrealx;
	realx = startrealy;
}

Particle::~Particle() {
	SDL_DestroyTexture(texture);
}

void Particle::render() {
	double delta_t = SDL_GetTicks() - timestamp_creation;

	//if(delta_t > 500) {

		// cout << "own: " << ownMass << endl;

		// cout << "center: " << CenterMass << endl;

	// cout << "realx: " << realx << endl;
	// cout << "realy: " << realy << endl;

	r = sqrt( pow(realx-250,2)+pow((500-realy)-(500-250),2));

	if(r<10) {
			rect.x = 600;
			rect.y = 600;
			return;
		}
	//r *= 10;

	// cout << "r: " << r << endl;

	F = G*(ownMass*CenterMass)/r;

	// cout << "F: " << F << endl;

	a = F / ownMass;

	if(a>10) {
				rect.x = 600;
				rect.y = 600;
				a = 0;
				return;
			}

	//a *= 1000;

	//a = 1;

	// cout << "a: " << a << endl;

	//gravalpha = SafeAcos((realx-250)/r);

	if((500-realy) - (500-250) != 0 || realx - 250 != 0)
		gravalpha = atan2((500-realy) - (500-250), realx - 250);

	// cout << "gravalpha: " << gravalpha << endl;

	ax = a*cos(gravalpha);

	ay = a*sin(gravalpha);

	realx = 0.5 * -ax * pow(delta_t / 1000., 2) + delta_t / 1000. * x_velocity + startrealx;
	realy = 0.5 * ay * pow(delta_t / 1000., 2) + delta_t / 1000. * y_velocity + startrealy;

	// cout << "ax: " << ax << endl;
	// cout << "ay: " << ay << endl;
	// cout << "pow: " << pow(delta_t / 1000., 2) << endl;
	// cout << "delta: " << delta_t << endl;
	// cout << "deltamal: " << delta_t / 1000. * x_velocity<< endl;
	// cout << "startrealx: " << startrealx << endl;

	// cout << "x: " << realx << endl;
	// cout << "y: " << realy << endl;

	rect.x = (int) realx;
	rect.y = (int) realy;

	realx = rect.x;
	realy = rect.y;



	SDL_RenderCopy(renderer, texture, NULL, &rect);

	// cout << endl;
	//}
}

bool Particle::isFaded() {
	return faded;
}

SDL_Rect* Particle::getRect() {
	return &rect;
}

double Particle::SafeAcos (double x)
  {
  if (x < -1.0)
	  x = -1.0 ;
  else if (x > 1.0)
	  x = 1.0 ;

  return acos (x) ;
  }

