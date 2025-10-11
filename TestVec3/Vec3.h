#pragma once
#include <iostream>
#include <cmath>

template<typename T>
class Vec3
{
public:
	T x;
	T y;
	T z;

	//Constructores

	Vec3();
	Vec3(T x, T y, T z);
	Vec3(const Vec3<T>& vec);

	//Operator

	//Funcionamiento operator:
	//c = a + b; == c = a.operator+(b);
	Vec3<T> operator+(const Vec3<T>& rhs) const; //rhs = right-hand side (2o operando)
	Vec3<T> operator-(const Vec3<T>& rhs) const;
	Vec3<T> operator*(const T& scalar) const;
	Vec3<T> operator/(const T& scalar) const;

	/* Esto no lo pide pero seria util
	Vec3<T>& operator+=(const Vec3<T>& rhs);
	Vec3<T>& operator-=(const Vec3<T>& rhs);
	Vec3<T>& operator*=(const T& scalar);
	Vec3<T>& operator/=(const T& scalar);

	bool operator==(const Vec3<T>& rhs) const;
	bool operator!=(const Vec3<T>& rhs) const;
	*/

	Vec3<T>& Normalize();
	T distance_to(const Vec3<T>& rhs) const;
	T dot_product(const Vec3<T>& rhs) const;
	Vec3<T> cross_product(const Vec3<T>& rhs) const;
	T angle_between(const Vec3<T>& rhs) const;
};

template<typename T>
Vec3<T>::Vec3() : x(0), y(0), z(0) {}

template<typename T>
Vec3<T>::Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

template<typename T>
Vec3<T>::Vec3(const Vec3<T>& vec) : x(vec.x), y(vec.y), z(vec.z) {}

template<typename T>
Vec3<T> Vec3<T>::operator+(const Vec3<T>& rhs) const
{
	return Vec3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
}

template<typename T>
Vec3<T> Vec3<T>::operator-(const Vec3<T>& rhs) const
{
	return Vec3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
}

template<typename T>
Vec3<T> Vec3<T>::operator*(const T& scalar) const
{
	return Vec3<T>(x * scalar, y * scalar, z * scalar);
}

template<typename T>
Vec3<T> Vec3<T>::operator/(const T& scalar) const
{
	return Vec3<T>(x / scalar, y / scalar, z / scalar);
}

template<typename T>
Vec3<T>& Vec3<T>::Normalize()
{
	T modulo = std::sqrt(x * x + y * y + z * z);
	if (modulo != 0)
	{
		x /= modulo;
		y /= modulo;
		z /= modulo;
	}
	return *this;
}

template<typename T>
T Vec3<T>::distance_to(const Vec3<T>& rhs) const
{
	Vec3<T> diff = *this - rhs;
	return std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
}

template<typename T>
T Vec3<T>::dot_product(const Vec3<T>& rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

template<typename T>
Vec3<T> Vec3<T>::cross_product(const Vec3<T>& rhs) const
{
	return Vec3<T>(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

template<typename T>
T Vec3<T>::angle_between(const Vec3<T>& rhs) const
{
	T dot = dot_product(rhs);
	T modulo_v = std::sqrt(x * x + y * y + z * z);
	T modulo_w = std::sqrt(rhs.x * rhs.x + rhs.y * rhs.y + rhs.z * rhs.z);
	T modulo = modulo_v * modulo_w;
	if (modulo == 0) return 0;

	//Debido a errores de precisión de punto flotante, el valor de dot / modulo podría ser ligeramente mayor que 1 o menor que -1.
	T cosAngle = dot / modulo;
	if (cosAngle > 1) cosAngle = 1;
	if (cosAngle < -1) cosAngle = -1;

	return std::acos(cosAngle);
}