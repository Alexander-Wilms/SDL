// Compile with -O3 (optimize most) for a better frame rate

#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <SDL.h>
#include "Display.h"

#define SIZE 500

using std::cout;
using std::endl;
using std::vector;

Display display(SIZE);

vector<vector<double>> data(SIZE, vector<double>(SIZE));

double choose_color(double iterations, double max_iter) {
	return (double) (255 * ((float) iterations) / max_iter);
}

double Julia(double x, double y, double xadd, double yadd, double max_betrag_2, double max_iter) {
	double remain_iter = max_iter;
	double xx = x * x;
	double yy = y * y;
	double xy = x * y;
	double absolute_value_2 = xx + yy;

	while (absolute_value_2 <= max_betrag_2 && remain_iter > 0) {
		remain_iter = remain_iter - 1;
		x = xx - yy + xadd;
		y = xy + xy + yadd;
		xx = x * x;
		yy = y * y;
		xy = x * y;
		absolute_value_2 = xx + yy;
	}

	return max_iter - remain_iter;
}

void apfel(double re_min, double im_min, double re_max, double im_max, double max_betrag_2,
		double xpixels, double ypixels, double max_iter) {
	for(int y = 0; y < ypixels; y++) {
		double c_im = im_min + (im_max - im_min) * y / ypixels;

		for(int x = 0; x < xpixels; x++) {
			double c_re = re_min + (re_max - re_min) * x / xpixels;

			double iterationen = Julia(c_re, c_im, c_re, c_im, max_betrag_2,
					max_iter);
			double color = choose_color(iterationen, max_iter);
			data[x][y] = color;
		}
	}
}

int main(int argc, char *argv[]) {
	for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE; j++) {
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

		apfel(re_min, im_min, re_max, im_max, max_betrag_2, SIZE, SIZE,
				max_iter);

		display.setdata(data);

		display.updateGraphics();

		SDL_Delay(5000);

	return 0;
}
