#pragma once
#include <platyvg.hpp>
#include "../core/layer.hpp"
#include "../core/record.hpp"
#include "../element/rect.hpp"
#include "../element/zoom.hpp"

namespace PlatyVGCore
{
	//! \brief 자료관리
	class Document
	{
	public:
		Document(const bool focusable);
		~Document();

	public:
		void UpdateLayers(const float radius);

	public:
		const sint32 ID;
		const bool Focusable;
		String Name;
		Rect Area;
		Zoom Transform;
		Layers AllLayers;
		Record ShapeRec;
		sint32 LastMS;
		Document* Next;
	};
	typedef Array<Document> Documents;
}
