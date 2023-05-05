#pragma once
#include "raylib.h"

struct Point
{
	int X;
	int Y;

	Point() : X(0), Y(0) { }
	Point(const int& x) : X(x), Y(x) { }
	Point(const int& x, const int& y) : X(x), Y(y) { }
	Point(Vector2 vector) : X(static_cast<int>(vector.x)), Y(static_cast<int>(vector.y)) { }
	Point(Vector2& vector) : X(static_cast<int>(vector.x)), Y(static_cast<int>(vector.y)) { }

	static Point FromIndex(const int& index, const int& size) {
		return { index % size, index / size };
	}
};

Point operator+ (const Point& p1, const Point& p2);
Point operator- (const Point& p1, const Point& p2);
Point operator* (const Point& point, const int& factor);
Point operator/ (const Point& point, const int& divisor);

struct IntRect
{
	Point Location;
	Point Size;

	bool Contains(Point& point) const {
		if (Location.X <= point.X && point.X < Location.X + Size.X && Location.Y <= point.Y)
		{
			return point.Y < Location.Y + Size.Y;
		}

		return false;
	}
};