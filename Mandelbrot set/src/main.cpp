// Compile with -O3 (optimize most) for a better frame rate

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include <vector>
#include <SDL.h>
#include "Display.h"
#include <complex>

#define XSIZE 300*2
#define YSIZE 200*2

// Standard Mandelbrot set
#define d 2
// #define d 2.5

//#define XSIZE 1600
//#define YSIZE 900

using std::cout;
using std::endl;
using std::vector;

bool running = true;

Display display(XSIZE, YSIZE);

vector<vector<double>> data(XSIZE, vector<double>(YSIZE));

int choose_color(double iterations, double max_iter) {
	// greyscale
	return (int) (255 * iterations / max_iter);

	// rainbow
	// return (int) (360 * ((float) iterations) / max_iter);

	// blue gradient
	// return (int) (((((float) iterations) / max_iter)*(240-180)+180));
}

double Julia(std::complex<double> input_c, double bound_magnitude, double max_iterations) {

	double remaining_iterations = max_iterations;

	// https://en.wikipedia.org/wiki/Mandelbrot_set#Formal_definition

	// c ist der betrachtete Punkt in der komplexen Ebene
	std::complex<double> c = input_c;

	std::complex<double> z_0 = std::complex<double>(0, 0);

	std::complex<double> z_n = (pow(z_0,d) + c);

	double magnitude = std::abs(pow(z_n,d));

	SDL_Event event;

	while (magnitude <= bound_magnitude && remaining_iterations > 0) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				}
			}
		}

		remaining_iterations = remaining_iterations - 1;

		z_n = (pow(z_n,d) + c);

		magnitude = abs(z_n);
	}

	//return max_iterations - remaining_iterations;

	// continuous color gradient
	return (double) ((double) max_iterations - remaining_iterations - log(log(bound_magnitude) / log(4)) / log(2));
}

void mandelbrot(double re_min, double im_min, double re_max, double im_max,
		double bound_magnitude, double xpixels, double ypixels, double max_iterations) {

	std::complex<double> c = std::complex<double>(0,0);

	for (int y = 0; y < ypixels; y++) {
		c.imag(im_min + (im_max - im_min) * y / ypixels);
		for (int x = 0; x < xpixels; x++) {
			c.real(re_min + (re_max - re_min) * x / xpixels);
			double iterations = Julia(c, bound_magnitude, max_iterations);

			int color = choose_color(iterations, max_iterations);
			data[x][y] = color;
		}
	}
}

int main(int argc, char *argv[]) {
	for (int i = 0; i < XSIZE; i++) {
		for (int j = 0; j < YSIZE; j++) {
			data[i][j] = -1;
		}
	}

	// https://de.wikipedia.org/wiki/Mandelbrot-Menge#Programmbeispiel

	double re_min = -2;
	double im_min = -1;
	double re_max = 1;
	double im_max = 1;
	double max_betrag = 4;
	double max_iterations = 1;

	while (running) {
		mandelbrot(re_min, im_min, re_max, im_max, max_betrag, XSIZE, YSIZE,
				max_iterations);

		cout << "max_iterations: " << max_iterations << endl;

		max_iterations++;

		display.setdata(data);

		display.updateGraphics();

	}
	return 0;
}
