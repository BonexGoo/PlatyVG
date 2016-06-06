#include <platyvg.hpp>
#include "coord.hpp"

namespace PlatyVGElement
{
	Coord::Coord()
	{
		x = 0;
		y = 0;
		force = 0;
	}

	Coord::Coord(const Coord& rhs)
	{
		operator=(rhs);
	}

	Coord::Coord(float x, float y, float force)
	{
		this->x = x;
		this->y = y;
		this->force = force;
	}

	Coord::~Coord()
	{
	}

	Coord& Coord::operator=(const Coord& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		force = rhs.force;
		return *this;
	}
}
