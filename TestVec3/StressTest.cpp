#include <iostream>
#include <cmath>
#include "Vec3.h"

using namespace std;

int main() {
    // ----------------------------------
    // Casos básicos
    // ----------------------------------
    Vec3<float> a(1, 2, 3);
    Vec3<float> b(4, 5, 6);

    Vec3<float> sum = a + b;
	cout << "a + b = (" << sum.x << ", " << sum.y << ", " << sum.z << ")\n";

    Vec3<float> diff = a - b;
    cout << "a - b = (" << diff.x << ", " << diff.y << ", " << diff.z << ")\n";

    Vec3<float> scaled = a * 2.0f;
    cout << "a * 2 = (" << scaled.x << ", " << scaled.y << ", " << scaled.z << ")\n";

    float dot = a.dot_product(b);
    cout << "Dot(a, b) = " << dot << "\n";

    Vec3<float> cross = a.cross_product(b);
    cout << "Cross(a, b) = (" << cross.x << ", " << cross.y << ", " << cross.z << ")\n";

    Vec3<float> normA = a;
    normA.Normalize();
    cout << "Normalize(a) = (" << normA.x << ", " << normA.y << ", " << normA.z << ")\n";

    float dist = a.distance_to(b);
    cout << "Distance(a, b) = " << dist << "\n";

    float angle = a.angle_between(b);
    cout << "Angle between a and b (rad) = " << angle << "\n";
    cout << "Angle between a and b (deg) = " << angle * 180.0f / 3.14159265f << "\n";

    // ----------------------------------
    // Casos límite
    // ----------------------------------
    Vec3<float> zero(0, 0, 0);
    cout << "\nTest vector cero:" << endl;
    zero.Normalize(); // Debe manejarse sin crash
    cout << "Normalize(zero) = (" << zero.x << ", " << zero.y << ", " << zero.z << ")\n";

    Vec3<float> parallel1(1, 1, 1);
    Vec3<float> parallel2(2, 2, 2);
    cout << "\nVectores paralelos:" << endl;
    cout << "Angle between parallel1 and parallel2 (deg) = "
        << parallel1.angle_between(parallel2) * 180.0f / 3.14159265f << "\n";

    Vec3<float> cross_parallel = parallel1.cross_product(parallel2);
    cout << "Cross(parallel1, parallel2) = ("
        << cross_parallel.x << ", " << cross_parallel.y << ", " << cross_parallel.z << ")\n";

    // ----------------------------------
    // Escalar 0 y negativo
    // ----------------------------------
    cout << "\nEscalares:" << endl;
    Vec3<float> zero_scaled = a * 0.0f;
    cout << "a * 0 = (" << zero_scaled.x << ", " << zero_scaled.y << ", " << zero_scaled.z << ")\n";

    Vec3<float> neg_scaled = a * -2.0f;
    cout << "a * -2 = (" << neg_scaled.x << ", " << neg_scaled.y << ", " << neg_scaled.z << ")\n";

    return 0;
}
