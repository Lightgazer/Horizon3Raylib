#pragma once
#include "raylib.h"

struct Point;
struct IntRect;

struct Point
{
	int X;
	int Y;

	Point() : X(0), Y(0) { }
	Point(const int x) : X(x), Y(x) { }
	Point(const int x, const int y) : X(x), Y(y) { }
	Point(const Vector2 vector) : X(static_cast<int>(vector.x)), Y(static_cast<int>(vector.y)) { }

	static Point FromIndex(const int index, const int size) {
		return { index % size, index / size };
	}

	int ToIndex(const int size) const {
		return Y * size + X;
	}

	Vector2 ToVector2() const {
		return { static_cast<float>(X), static_cast<float>(Y) };
	}
};

Point operator+ (const Point p1, const Point p2);
Point operator+ (const Point point);
Point& operator+= (Point& p1, const Point p2);
Point operator- (const Point p1, const Point p2);
Point operator- (const Point point);
Point& operator-= (Point& p1, const Point p2);
Point operator* (const Point point, const int factor);
Point operator/ (const Point point, const int divisor);
bool operator== (const Point p1, const Point p2);

Vector2 operator+ (const Vector2 v1, const Vector2 v2);
Vector2 operator+ (const Vector2 vector);
Vector2& operator+= (Vector2& v1, const Vector2 v2);
Vector2 operator- (const Vector2 v1, const Vector2 v2);
Vector2 operator- (const Vector2 vector);
Vector2& operator-= (Vector2& v1, const Vector2 v2);
Vector2 operator* (const Vector2 vector, const float factor);
Vector2 operator/ (const Vector2 vector, const float divisor);

struct IntRect
{
	Point Location;
	Point Size;

	bool Contains(const Point point) const {
		if (Location.X <= point.X && point.X < Location.X + Size.X && Location.Y <= point.Y) 
		{
			return point.Y < Location.Y + Size.Y;
		}

		return false;
	}
};
