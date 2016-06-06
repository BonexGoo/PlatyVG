#pragma once
#include <platyvg.hpp>
#include "../element/matrix.hpp"

namespace PlatyVGElement
{
	//! \brief 2D좌표
	class Point
	{
	public:
		Point();
		Point(const Point& rhs);
		Point(float x, float y);
		~Point();

		Point& operator=(const Point& rhs);
		Point& operator*=(const Matrix& rhs);
		Point operator*(const Matrix& rhs) const;
		bool operator==(const Point& rhs) const;
		bool operator!=(const Point& rhs) const;

		const bool ClockwiseTest(const Point begin, const Point end) const;

	public:
		float x;
		float y;
	};
	typedef Array<Point, true> Points;
}
