#pragma once
#include <platyvg.hpp>

namespace PlatyVGElement
{
	//! \brief 색상정보
	class Color
	{
	public:
		Color();
		Color(const Color& rhs);
		Color(uint32 rgba, uint32 aqua);
		Color(uint08 r, uint08 g, uint08 b, uint08 a, uint32 aqua);
		~Color();
		Color& operator=(const Color& rhs);

	public:
		union
		{
			uint32 rgba;
			struct
			{
				uint08 r;
				uint08 g;
				uint08 b;
				uint08 a;
			};
		};
		uint32 aqua;
	};
}
