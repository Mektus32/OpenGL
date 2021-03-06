#pragma once

#include <cmath>
#include <vector>


struct point {
	point() : x(0), y(0), z(0) {};
	point(double x, double y, double z) : x(x), y(y), z(z) {};
	point(const point& p) = default;
	double x;
	double y;
	double z;

	operator double* () {
		return (double*)this;
	}

	point normaliz() const {
		return { this->x / len(), this->y / len(), this->z / len() };
	}

	double len() const {
		return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
	}
};

struct vector {
	vector(point p) : x(p.x), y(p.y), z(p.z) {}
	vector(double x, double y, double z) : x(x), y(y), z(z) {}
	double x;
	double y;
	double z;
};

point operator*(const point& lhs, double rhs) {
	return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}

point operator*=(point& lhs, double rhs) {
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	return lhs;
}

point operator*(double lhs, const point& rhs) {
	return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}

point operator+(const point& lhs, const point& rhs) {
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

point operator-(const point& lhs, const point& rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

void operator+=(point& lhs, const point& rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
}
