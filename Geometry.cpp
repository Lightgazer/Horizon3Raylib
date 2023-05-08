#pragma once
#include "Geometry.h"

Point operator+ (const Point p1, const Point p2)
{
	return { p1.X + p2.X, p1.Y + p2.Y };
}

Point operator+(const Point point)
{
	return point;
}

Point& operator+=(Point& p1, const Point p2)
{
	p1.X += p2.X;
	p1.Y += p2.Y;
	return p1;
}

Point operator- (const Point p1, const Point p2)
{
	return { p1.X - p2.X, p1.Y - p2.Y };
}

Point operator-(const Point point)
{
	return { -point.X, -point.Y };
}

Point& operator-=(Point& p1, const Point p2)
{
	p1.X -= p2.X;
	p1.Y -= p2.Y;
	return p1;
}

Point operator* (const Point point, const int factor)
{
	return { point.X * factor, point.Y * factor };
}

Point operator/ (const Point point, const int divisor)
{
	return { point.X / divisor, point.Y / divisor };
}

Vector2 operator+(const Vector2 v1, const Vector2 v2)
{
	return { v1.x + v2.x, v1.y + v2.y };
}

Vector2 operator+(const Vector2 vector)
{
	return vector;
}

Vector2& operator+=(Vector2& v1, const Vector2 v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}

Vector2 operator- (const Vector2 v1, const Vector2 v2)
{
	return { v1.x - v2.x, v1.y - v2.y };
}

Vector2 operator-(const Vector2 vector)
{
	return { -vector.x, -vector.y };
}

Vector2& operator-=(Vector2& v1, const Vector2 v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	return v1;
}

Vector2 operator*(const Vector2 vector, const float factor)
{
	return { vector.x * factor, vector.y * factor };;
}

Vector2 operator/(const Vector2 vector, const float divisor)
{
	return { vector.x / divisor, vector.y / divisor };;
}
