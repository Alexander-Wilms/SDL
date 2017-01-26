// https://forum.kde.org/viewtopic.php?f=74&t=94694

#define XSIZE 150
#define YSIZE 150
#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <SDL.h>
#include "Display.h"
#include <vector>

using namespace Eigen;
using std::cout;
using std::cin;
using std::endl;
using std::vector;

Display display(XSIZE, YSIZE);

vector<vector<double>> data(XSIZE, vector<double>(YSIZE));

Hyperplane<double, 3> plane;

double getDepth(int x, int y);

int main(int argc, char *argv[]) {

	Vector3d vb(0, 0, 0);
	Vector3d vc(1, 0, 0);
	Vector3d vd(0, 1, 1);
	plane = Hyperplane<double, 3>::Through(vb, vc, vd);

	for (int x = 0; x < XSIZE; x++) {
		for (int y = 0; y < YSIZE; y++) {

			data[x][y] = getDepth(x,y);
		}
	}

	display.setdata(data);

	display.updateGraphics();

	SDL_Event event;

	while (true) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			break;
		}
	}

	return 0;
}

double getDepth(int x, int y) {
	Vector3d va(x, y, 0);
	Vector3d vb(x, y, 1);
	ParametrizedLine<double, 3> line = ParametrizedLine<double, 3>::Through(va, vb);

	return line.intersection(plane);
}
