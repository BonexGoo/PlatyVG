#include <platyvg.hpp>
#include "context.hpp"

namespace PlatyVGElement
{
	Context::Context()
	{
		thick = 0;
	}

	Context::Context(const Context& rhs)
	{
		operator=(rhs);
	}

	Context::Context(const Color& color, const float thick)
	{
		this->color = color;
		this->thick = thick;
	}

	Context::~Context()
	{
	}

	Context& Context::operator=(const Context& rhs)
	{
		color = rhs.color;
		thick = rhs.thick;
		return *this;
	}
}
