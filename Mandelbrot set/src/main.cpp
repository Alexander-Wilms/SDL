// Compile with -O3 (optimize most) for a better frame rate

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include <vector>
#include <SDL.h>
#include "Display.h"

//#define XSIZE 300*2
//#define YSIZE 200*2

#define XSIZE 1600
#define YSIZE 900

using std::cout;
using std::endl;
using std::vector;

Display display(XSIZE, YSIZE);

vector<vector<double>> data(XSIZE, vector<double>(YSIZE));

int choose_color(double iterations, double max_iter) {
	// greyscale
	 return (int) (255 * iterations / max_iter);

	// rainbow
	//return (int) (360 * ((float) iterations) / max_iter);

	// blue gradient
	//return (int) (((((float) iterations) / max_iter)*(240-180)+180));
}

double Julia(double x, double y, double xadd, double yadd, double max_betrag_2,
		double max_iter) {
	double remain_iter = max_iter;
	double xx = x * x;
	double yy = y * y;
	double xy = x * y;
	double absolute_value_2 = xx + yy;

	SDL_Event event;

	while (absolute_value_2 <= max_betrag_2 && remain_iter > 0) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				return -1;
			}
		}

		remain_iter = remain_iter - 1;
		x = xx - yy + xadd;
		y = xy + xy + yadd;
		xx = x * x;
		yy = y * y;
		xy = x * y;
		absolute_value_2 = xx + yy;
	}

	return max_iter - remain_iter;

	// continuous color gradient
	//return (double) ((double) max_iter - remain_iter - log(log(absolute_value_2) / log(4)) / log(2));
}

void apfel(double re_min, double im_min, double re_max, double im_max,
		double max_betrag_2, double xpixels, double ypixels, double max_iter) {

	for (int y = 0; y < ypixels; y++) {
		double c_im = im_min + (im_max - im_min) * y / ypixels;

		for (int x = 0; x < xpixels; x++) {

			double c_re = re_min + (re_max - re_min) * x / xpixels;

			double iterationen = Julia(c_re, c_im, c_re, c_im, max_betrag_2,
					max_iter);

			int color = choose_color(iterationen, max_iter);
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
	double max_betrag_2 = 4;
	double max_iter = 100;

	bool running = true;
	SDL_Event event;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}

		apfel(re_min, im_min, re_max, im_max, max_betrag_2, XSIZE, YSIZE,
				max_iter);

		cout << "max_iter: " << max_iter << endl;
		max_iter++;

		display.setdata(data);

		display.updateGraphics();

	}
	return 0;
}
