#pragma once
#include <platyvg.hpp>

namespace PlatyVGElement
{
	//! \brief 터치좌표
	class Coord
	{
	public:
		Coord();
		Coord(const Coord& rhs);
		Coord(float x, float y, float force);
		~Coord();
		Coord& operator=(const Coord& rhs);

	public:
		float x;
		float y;
		float force;
	};
	typedef Array<Coord, true> Coords;
}
