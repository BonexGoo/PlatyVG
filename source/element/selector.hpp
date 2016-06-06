#pragma once
#include <platyvg.hpp>
#include "../element/coord.hpp"
#include "../element/point.hpp"
#include "../element/rect.hpp"
#include "../element/table.hpp"

namespace PlatyVGElement
{
	//! \brief 선택자
	class Selector
	{
	public:
		Selector(const Coords coods);
		~Selector();

	public:
		const bool IsValid() const;
		const Rect GetRect() const;
		const Rects GetRects();
		const Points GetPoints();
		const Tables GetTables(sint32* ymin);

	private:
		const Coords coods;
		Rects rects;
		Points points;
		Tables tables;
		sint32 tables_ymin;
	};
}
