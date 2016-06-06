#pragma once
#include <platyvg.hpp>
#include "../element/point.hpp"

namespace PlatyVGElement
{
	//! \brief 기점좌표
	class Edge
	{
	public:
		Edge();
		Edge(const Edge& rhs);
		virtual ~Edge();
		Edge& operator=(const Edge& rhs);

	public:
		virtual bool IsValid() const;
		virtual const Point ValidPoint() const;

	public:
		virtual sint32 LengthOfWrite() const;
		virtual void Write(uint08* ptr) const;
	};

	//! \brief 기점좌표(Spline)
	class EdgeSpline : public Edge
	{
	public:
		EdgeSpline();
		EdgeSpline(const EdgeSpline& rhs);
		EdgeSpline(float x, float y);
		EdgeSpline(bytes rhs);
		virtual ~EdgeSpline();
		EdgeSpline& operator=(const EdgeSpline& rhs);

	public:
		virtual bool IsValid() const;
		virtual const Point ValidPoint() const;

	public:
		virtual sint32 LengthOfWrite() const;
		virtual void Write(uint08* ptr) const;

	public:
		float x;
		float y;
	};
	typedef Array<EdgeSpline, true> EdgeSplines;

	//! \brief 기점좌표(Tube)
	class EdgeTube : public Edge
	{
	public:
		EdgeTube();
		EdgeTube(const EdgeTube& rhs);
		EdgeTube(float force, float lx, float ly, float rx, float ry);
		EdgeTube(bytes rhs);
		virtual ~EdgeTube();
		EdgeTube& operator=(const EdgeTube& rhs);

	public:
		virtual bool IsValid() const;
		virtual const Point ValidPoint() const;

	public:
		virtual sint32 LengthOfWrite() const;
		virtual void Write(uint08* ptr) const;

	public:
		bool endmark;
		float force;
		float lx;
		float ly;
		float rx;
		float ry;
	};
	typedef Array<EdgeTube, true> EdgeTubes;
}
