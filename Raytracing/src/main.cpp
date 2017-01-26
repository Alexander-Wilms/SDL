// https://forum.kde.org/viewtopic.php?f=74&t=94694

#define SIZE 100
#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Geometry/Hyperplane.h>
#include <Eigen/src/Geometry/ParametrizedLine.h>
#include <SDL_main.h>
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <SDL.h>
#include "Display.h"
#include <vector>

using namespace Eigen;
using std::cout;
using std::endl;
using std::vector;

Display display(SIZE, SIZE);

vector<vector<double>> data(SIZE, vector<double>(SIZE));

int main(int argc, char *argv[]) {
	Vector3d vz(0, 0, 2);
	Vector3d v0(0, 0, 0);
	ParametrizedLine<double, 3> line = ParametrizedLine<double, 3>::Through(vz, v0);

	Vector3d vx(1, 0, 0);
	Vector3d vy(0, 1, 0);
	Hyperplane<double, 3> plane = Hyperplane<double, 3>::Through(v0, vx, vy);

	double intersection_parameter = line.intersection(plane);

	cout << intersection_parameter << endl;

	return 0;
}
