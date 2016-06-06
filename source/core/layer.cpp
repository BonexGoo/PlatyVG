#include <platyvg.hpp>
#include "layer.hpp"

namespace PlatyVGCore
{
	Layer::Layer()
	{
		Visible = true;
		LockedFlag = 0;
	}

	Layer::~Layer()
	{
	}

	void Layer::UpdateShapes(const Zoom& transform, const float radius)
	{
		for(sint32 i = 0, iend = AllShapes.Count(); i < iend; ++i)
		{
			Shape* CurShape = AllShapes.At(i).ptr();
			CurShape->Update(transform.offset, transform.scale, radius);
		}
	}
}
