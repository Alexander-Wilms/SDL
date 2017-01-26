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

vector<vector<SDL_Color>> data(XSIZE, vector<SDL_Color>(YSIZE));

Hyperplane<double, 3> plane1;
Hyperplane<double, 3> plane2;

SDL_Color cast_ray(double x, double y);

int main(int argc, char *argv[]) {

	Vector3d vb(0, 0, 100);
	Vector3d vc(1, 0, 100);
	Vector3d vd(0, 1, 100);

	Vector3d ve(1, 0, 3);
	Vector3d vf(0, 1, 3);
	Vector3d vg(0, 0, 2);

	plane1 = Hyperplane<double, 3>::Through(vb, vc, vd);

	plane2 = Hyperplane<double, 3>::Through(ve, vf, vg);

	for (int x = 0; x < XSIZE; x++) {
		for (int y = 0; y < YSIZE; y++) {

			data[x][y] = cast_ray(x, y);
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

SDL_Color cast_ray(double x, double y) {
	SDL_Color returnvalue;
	returnvalue.r = 0;
	returnvalue.g = 0;
	returnvalue.b = 0;

	Vector3d va(x, y, 0);
	Vector3d vb(x, y, 1);
	ParametrizedLine<double, 3> ray = ParametrizedLine<double, 3>::Through(va, vb);

	Vector3d plane1_distance = ray.intersectionPoint(plane1);
	Vector3d plane2_distance = ray.intersectionPoint(plane2);

	double* plane1_d = &plane1_distance(2);
	double* plane2_d = &plane2_distance(2);

	//cout << "plane1: " << *plane1_d << endl;
	//cout << "plane2: " << *plane2_d << endl;

	if (*plane1_d < *plane2_d) {
		returnvalue.r = *plane1_d;
		//cout << *plane1_d << endl;
		//cout << returnvalue.r << endl;
	} else {
		returnvalue.g = *plane2_d;
		//cout << *plane2_d << endl;
		//cout << returnvalue.g << endl;
	}

	double x0 = 50;
	double y0 = 50;
	double z0 = 50;
	double r = 50;

	Vector3d center_of_sphere(x0, y0, z0);
	Vector3d raypoint;
	int t = 0;



	if (ray.distance(center_of_sphere) < r) {
		double z = sqrt(pow(r,2)-pow((x-x0),2)-pow((y-y0),2))+z0;
		cout << "ray hits sphere: " << z << endl;
		returnvalue.r = z;
		returnvalue.g = z;
		returnvalue.b = z;
	}

	return returnvalue;
}
