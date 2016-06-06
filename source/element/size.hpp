#pragma once
#include <platyvg.hpp>

namespace PlatyVGElement
{
	//! \brief 2D구간
	class Size
	{
	public:
		Size();
		Size(const Size& rhs);
		Size(float w, float h);
		~Size();
		Size& operator=(const Size& rhs);

	public:
		float w;
		float h;
	};
}
