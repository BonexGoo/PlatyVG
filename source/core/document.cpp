#include <platyvg.hpp>
#include "document.hpp"

static sint32 LastID = -1;

namespace PlatyVGCore
{
	Document::Document(const bool focusable) : ID(LastID++), Focusable(focusable)
	{
		LastMS = -1;
		Next = nullptr;
	}

	Document::~Document()
	{
		delete Next;
	}

	void Document::UpdateLayers(const float radius)
	{
		for(sint32 i = 0, iend = AllLayers.Count(); i < iend; ++i)
			AllLayers.At(i).UpdateShapes(Transform, radius);
	}
}
