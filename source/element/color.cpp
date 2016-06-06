#include <platyvg.hpp>
#include "color.hpp"

namespace PlatyVGElement
{
	Color::Color()
	{
		rgba = 0;
		aqua = 0;
	}

	Color::Color(const Color& rhs)
	{
		operator=(rhs);
	}

	Color::Color(uint32 rgba, uint32 aqua)
	{
		this->rgba = rgba;
		this->aqua = aqua;
	}

	Color::Color(uint08 r, uint08 g, uint08 b, uint08 a, uint32 aqua)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
		this->aqua = aqua;
	}

	Color::~Color()
	{
	}

	Color& Color::operator=(const Color& rhs)
	{
		rgba = rhs.rgba;
		aqua = rhs.aqua;
		return *this;
	}
}
