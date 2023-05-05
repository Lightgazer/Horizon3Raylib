#pragma once
#include "Point.h"

Point operator+ (const Point& p1, const Point& p2)
{
	return { p1.X + p2.X, p1.Y + p2.Y };
}

Point operator- (const Point& p1, const Point& p2)
{
	return { p1.X - p2.X, p1.Y - p2.Y };
}

Point operator* (const Point& point, const int& factor)
{
	return { point.X * factor, point.Y * factor };
}

Point operator/ (const Point& point, const int& divisor)
{
	return { point.X / divisor, point.Y / divisor };
}