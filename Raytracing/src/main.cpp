// https://forum.kde.org/viewtopic.php?f=74&t=94694

#define XSIZE 100
#define YSIZE 100
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

Hyperplane<double, 3> plane1;
Hyperplane<double, 3> plane2;

int getDepth(double x, double y);

int main(int argc, char *argv[]) {

	Vector3d vb(0, 0, 150);
	Vector3d vc(1, 0, 150);
	Vector3d vd(0, 1, 150);

	Vector3d ve(1, 0, 3);
	Vector3d vf(0, 1, 3);
	Vector3d vg(0, 0, 2);

	plane1 = Hyperplane<double, 3>::Through(vb, vc, vd);

	plane2 = Hyperplane<double, 3>::Through(ve, vf, vg);

	double max = 0;

	for (int x = 0; x < XSIZE; x++) {
		for (int y = 0; y < YSIZE; y++) {

			data[x][y] = getDepth(x, y);
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

int getDepth(double x, double y) {
	Vector3d va(x, y, 0);
	Vector3d vb(x, y, 1);
	ParametrizedLine<double, 3> line = ParametrizedLine<double, 3>::Through(va, vb);

	int returnvalue;

	Vector3d plane1_distance = line.intersectionPoint(plane1);
	Vector3d plane2_distance = line.intersectionPoint(plane2);

	double* plane1_d = &plane1_distance(2);
	double* plane2_d = &plane2_distance(2);

	cout << "plane1: " << *plane1_d << endl;
	cout << "plane2: " << *plane2_d << endl;

	if(*plane1_d < *plane2_d) {
		returnvalue = *plane1_d;
		cout << *plane1_d << endl;
		cout << returnvalue << endl;
	} else {
		returnvalue = *plane2_d;
		cout << *plane2_d << endl;
		cout << returnvalue << endl;
	}



	return returnvalue;
}
