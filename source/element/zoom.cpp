#include <platyvg.hpp>
#include "zoom.hpp"

namespace PlatyVGElement
{
	Zoom::Zoom()
	{
		scale = 1;
	}

	Zoom::Zoom(const Zoom& rhs)
	{
		operator=(rhs);
	}

	Zoom::Zoom(Point offset, float scale)
	{
		this->offset = offset;
		this->scale = scale;
	}

	Zoom::~Zoom()
	{
	}

	Zoom& Zoom::operator=(const Zoom& rhs)
	{
		offset = rhs.offset;
		scale = rhs.scale;
		return *this;
	}

	bool Zoom::operator==(const Zoom& rhs) const
	{
		return (offset == rhs.offset && scale == rhs.scale);
	}

	bool Zoom::operator!=(const Zoom& rhs) const
	{
		return !operator==(rhs);
	}

	const Point Zoom::ToView(const Point point) const
	{
		return Point((point.x + offset.x) * scale, (point.y + offset.y) * scale);
	}

	const Point Zoom::ToView(const float x, const float y) const
	{
		return Point((x + offset.x) * scale, (y + offset.y) * scale);
	}

	const Point Zoom::ToDocument(const Point point) const
	{
		return Point(point.x / scale - offset.x, point.y / scale - offset.y);
	}

	const Point Zoom::ToDocument(const float x, const float y) const
	{
		return Point(x / scale - offset.x, y / scale - offset.y);
	}

	void Zoom::UpdateBy(const Point (&oldpoint)[2], const Point (&newpoint)[2])
	{
		const Point OldMidPos = Point((oldpoint[0].x + oldpoint[1].x) / 2, (oldpoint[0].y + oldpoint[1].y) / 2);
		const Point NewMidPos = Point((newpoint[0].x + newpoint[1].x) / 2, (newpoint[0].y + newpoint[1].y) / 2);
		const float OldDistance = Math::Distance(oldpoint[0].x, oldpoint[0].y, oldpoint[1].x, oldpoint[1].y);
		const float NewDistance = Math::Distance(newpoint[0].x, newpoint[0].y, newpoint[1].x, newpoint[1].y);
		const float NewScale = Math::MinF(Math::MaxF(0.0001f, scale * NewDistance / OldDistance), 1000.0f);
		offset = Point(
			offset.x + NewMidPos.x / NewScale - OldMidPos.x / scale,
			offset.y + NewMidPos.y / NewScale - OldMidPos.y / scale);
		scale = NewScale;
	}

	void Zoom::UpdateBy(const Point oldpoint, const Point newpoint, const float scaleadds)
	{
		const float NewScale = Math::MinF(Math::MaxF(0.0001f, scale * scaleadds), 1000.0f);
		offset = Point(
			offset.x + newpoint.x / NewScale - oldpoint.x / scale,
			offset.y + newpoint.y / NewScale - oldpoint.y / scale);
		scale = NewScale;
	}
}
