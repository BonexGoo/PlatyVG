#pragma once
#include <platyvg.hpp>
#include "../element/point.hpp"
#include "../element/size.hpp"
#include "../element/zoom.hpp"

namespace PlatyVGElement
{
	//! \brief 2D영역
	class Rect
	{
	public:
		Rect();
		Rect(const Rect& rhs);
		Rect(const Point point, const Size size);
		Rect(float l, float t, float r, float b);
		~Rect();

		Rect& operator=(const Rect& rhs);
		Rect& operator+=(const Point& rhs);
		Rect operator+(const Point& rhs) const;
		Rect& operator-=(const Point& rhs);
		Rect operator-(const Point& rhs) const;
		Rect& operator*=(const Zoom& rhs);
		Rect operator*(const Zoom& rhs) const;
		Rect& operator/=(const Zoom& rhs);
		Rect operator/(const Zoom& rhs) const;

		const float Width() const;
		const float Height() const;
		const float CenterX() const;
		const float CenterY() const;
		const bool PtInRect(const Point point) const;
		const bool PtInRect(const float x, const float y) const;
		const bool ContactTest(const Rect rect) const;
		Rect Inflate(const float x, const float y) const;
		Rect Deflate(const float x, const float y) const;

	public:
		float l;
		float t;
		float r;
		float b;
	};
	typedef Array<Rect, true> Rects;
}
