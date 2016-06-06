#pragma once
#include <platyvg.hpp>
#include "../core/shape.hpp"
#include "../element/zoom.hpp"

namespace PlatyVGCore
{
	//! \brief 레이어관리
	class Layer
	{
	public:
		Layer();
		~Layer();

	public:
		void UpdateShapes(const Zoom& transform, const float radius);

	public:
		UnionShapes AllShapes;
		UnionShapes DynShapes;
		bool Visible;
		uint32 LockedFlag;
	};
	typedef Array<Layer> Layers;
}
