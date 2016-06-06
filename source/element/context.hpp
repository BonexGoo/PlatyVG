#pragma once
#include <platyvg.hpp>
#include "../element/color.hpp"

namespace PlatyVGElement
{
	//! \brief 상태보유
	class Context
	{
	public:
		Context();
		Context(const Context& rhs);
		Context(const Color& color, const float thick);
		~Context();
		Context& operator=(const Context& rhs);

	public:
		Color color;
		float thick;
	};
}
