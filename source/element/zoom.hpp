#pragma once
#include <platyvg.hpp>
#include "../element/point.hpp"

namespace PlatyVGElement
{
	//! \brief 2D줌
	class Zoom
	{
	public:
		Zoom();
		Zoom(const Zoom& rhs);
		Zoom(Point offset, float scale);
		~Zoom();
		Zoom& operator=(const Zoom& rhs);
		bool operator==(const Zoom& rhs) const;
		bool operator!=(const Zoom& rhs) const;

		const Point ToView(const Point point) const;
		const Point ToView(const float x, const float y) const;
		const Point ToDocument(const Point point) const;
		const Point ToDocument(const float x, const float y) const;

		void UpdateBy(const Point (&oldpoint)[2], const Point (&newpoint)[2]);
		void UpdateBy(const Point oldpoint, const Point newpoint, const float scaleadds);

	public:
		Point offset;
		float scale;
	};
}
