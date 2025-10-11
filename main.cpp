#include<iostream>
#include"Vec3.h"
#include<cmath>

int main() {
	Vec3<int> a(1, 2, 3);
	Vec3<int> b(4, 5, 6);
	Vec3<int> c(b);

	Vec3<float> d = a + c;
	cout << "a + c = " << d << endl;
	cout << "d normalized = " << d.Normalize(); << endl;
	cout << "d distance to b = " << d.distance_to(b) << endl;
	cout << "d dot product b = " << d.dot_product(b) << endl;
	cout << "d cross product b = " << d.cross_product(b) << endl;
	cout << "d angle between b = " << d.angle_between(b) << endl;

	return 0;
}