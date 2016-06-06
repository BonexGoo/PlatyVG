#include <platyvg.hpp>
#include "size.hpp"

namespace PlatyVGElement
{
	Size::Size()
	{
		w = 0;
		h = 0;
	}

	Size::Size(const Size& rhs)
	{
		operator=(rhs);
	}

	Size::Size(float w, float h)
	{
		this->w = w;
		this->h = h;
	}

	Size::~Size()
	{
	}

	Size& Size::operator=(const Size& rhs)
	{
		w = rhs.w;
		h = rhs.h;
		return *this;
	}
}
