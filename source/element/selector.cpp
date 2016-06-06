#include <platyvg.hpp>
#include "selector.hpp"

namespace PlatyVGElement
{
	Selector::Selector(const Coords coods) : coods(coods)
	{
		tables_ymin = 0;
	}

	Selector::~Selector()
	{
	}

	const bool Selector::IsValid() const
	{
		return (1 < coods.Count());
	}

	const Rect Selector::GetRect() const
	{
		if(!IsValid()) return Rect();
		return Rect(coods[0].x, coods[0].y, coods[-1].x, coods[-1].y);
	}

	const Rects Selector::GetRects()
	{
		if(IsValid() && rects.Count() == 0)
		for(sint32 i = 0, iend = coods.Count() - 1; i < iend; ++i)
			rects.AtAdding() = Rect(coods[i].x, coods[i].y, coods[i + 1].x, coods[i + 1].y);
		return rects;
	}

	const Points Selector::GetPoints()
	{
		if(IsValid() && points.Count() == 0)
		for(sint32 i = 0, iend = coods.Count(); i < iend; ++i)
			points.AtAdding() = Point(coods[i].x, coods[i].y);
		return points;
	}

	const Tables Selector::GetTables(sint32* ymin)
	{
		if(IsValid() && tables.Count() == 0)
		{
			tables_ymin = (sint32) coods[0].y;
			for(sint32 i = 1, iend = coods.Count(); i < iend; ++i)
				tables_ymin = Math::Min(tables_ymin, (sint32) coods[i].y);

			for(sint32 i = 0, iend = coods.Count(); i < iend; ++i)
			{
				const sint32 X1 = (sint32) coods[i].x;
				const sint32 Y1 = (sint32) coods[i].y;
				const sint32 X2 = (sint32) coods[(i + 1) % iend].x;
				const sint32 Y2 = (sint32) coods[(i + 1) % iend].y;
				const bool IsAscend = (Y1 < Y2);
				const sint32 XB = (IsAscend)? X1 : X2;
				const sint32 YB = (IsAscend)? Y1 : Y2;
				const sint32 XE = X1 + X2 - XB;
				const sint32 YE = Y1 + Y2 - YB;
				const sint32 VectorX = XE - XB;
				const sint32 VectorY = YE - YB;
				for(sint32 y = YB; y < YE; ++y)
					tables.AtWherever(y - tables_ymin).row.AtAdding()
						= XB + VectorX * (y - YB + 1) / (VectorY + 1);
			}
		}
		if(ymin) *ymin = tables_ymin;
		return tables;
	}
}
