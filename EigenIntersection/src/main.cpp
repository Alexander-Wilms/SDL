// https://forum.kde.org/viewtopic.php?f=74&t=94694

#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Geometry>

using namespace Eigen;
using std::cout;
using std::endl;

int main()
{
	Vector3d vz(0,0,1);
	Vector3d v0(0,0,0);
	ParametrizedLine<double,3> line = ParametrizedLine<double,3>::Through(vz, v0);

	Vector3d vx(1,0,0);
	Vector3d vy(0,1,0);
	Hyperplane<double, 3> plane = Hyperplane<double, 3>::Through(v0, vx, vy);

	double intersection_parameter = line.intersection(plane);

	cout << intersection_parameter << endl;
}
