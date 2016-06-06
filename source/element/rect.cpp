#include <platyvg.hpp>
#include "rect.hpp"

namespace PlatyVGElement
{
	Rect::Rect()
	{
		l = 0;
		t = 0;
		r = 0;
		b = 0;
	}

	Rect::Rect(const Rect& rhs)
	{
		operator=(rhs);
	}

	Rect::Rect(const Point point, const Size size)
	{
		l = point.x;
		t = point.y;
		r = point.x + size.w;
		b = point.y + size.h;
	}

	Rect::Rect(float l, float t, float r, float b)
	{
		this->l = (l < r)? l : r;
		this->t = (t < b)? t : b;
		this->r = (l < r)? r : l;
		this->b = (t < b)? b : t;
	}

	Rect::~Rect()
	{
	}

	Rect& Rect::operator=(const Rect& rhs)
	{
		l = rhs.l;
		t = rhs.t;
		r = rhs.r;
		b = rhs.b;
		return *this;
	}

	Rect& Rect::operator+=(const Point& rhs)
	{
		l -= rhs.x;
		t -= rhs.y;
		r += rhs.x;
		b += rhs.y;
		return *this;
	}

	Rect Rect::operator+(const Point& rhs) const
	{
		return Rect(*this).operator+=(rhs);
	}

	Rect& Rect::operator-=(const Point& rhs)
	{
		l += rhs.x;
		t += rhs.y;
		r -= rhs.x;
		b -= rhs.y;
		return *this;
	}

	Rect Rect::operator-(const Point& rhs) const
	{
		return Rect(*this).operator-=(rhs);
	}

	Rect& Rect::operator*=(const Zoom& rhs)
	{
		const Point New_lt = rhs.ToView(l, t);
		const Point New_rb = rhs.ToView(r, b);
		l = New_lt.x;
		t = New_lt.y;
		r = New_rb.x;
		b = New_rb.y;
		return *this;
	}

	Rect Rect::operator*(const Zoom& rhs) const
	{
		return Rect(*this).operator*=(rhs);
	}

	Rect& Rect::operator/=(const Zoom& rhs)
	{
		const Point New_lt = rhs.ToDocument(l, t);
		const Point New_rb = rhs.ToDocument(r, b);
		l = New_lt.x;
		t = New_lt.y;
		r = New_rb.x;
		b = New_rb.y;
		return *this;
	}

	Rect Rect::operator/(const Zoom& rhs) const
	{
		return Rect(*this).operator/=(rhs);
	}

	const float Rect::Width() const
	{
		return r - l;
	}

	const float Rect::Height() const
	{
		return b - t;
	}

	const float Rect::CenterX() const
	{
		return (r + l) / 2;
	}

	const float Rect::CenterY() const
	{
		return (b + t) / 2;
	}

	const bool Rect::PtInRect(const Point point) const
	{
		return (l <= point.x && t <= point.y && point.x < r && point.y < b);
	}

	const bool Rect::PtInRect(const float x, const float y) const
	{
		return (l <= x && t <= y && x < r && y < b);
	}

	const bool Rect::ContactTest(const Rect rect) const
	{
		return l < rect.r && t < rect.b && rect.l < r && rect.t < b;
	}

	Rect Rect::Inflate(const float x, const float y) const
	{
		return Rect(*this).operator+=(Point(x, y));
	}

	Rect Rect::Deflate(const float x, const float y) const
	{
		return Rect(*this).operator-=(Point(x, y));
	}
}
