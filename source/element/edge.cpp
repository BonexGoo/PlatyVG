#include <platyvg.hpp>
#include "edge.hpp"

namespace PlatyVGElement
{
	Edge::Edge()
	{
	}

	Edge::Edge(const Edge& rhs)
	{
		operator=(rhs);
	}

	Edge::~Edge()
	{
	}

	Edge& Edge::operator=(const Edge& rhs)
	{
		return *this;
	}

	bool Edge::IsValid() const
	{
		return false;
	}

	const Point Edge::ValidPoint() const
	{
		return Point();
	}

	sint32 Edge::LengthOfWrite() const
	{
		return 0;
	}

	void Edge::Write(uint08* ptr) const
	{
	}

	//////////////////////////////////////////////////
	EdgeSpline::EdgeSpline()
	{
		x = 0;
		y = 0;
	}

	EdgeSpline::EdgeSpline(const EdgeSpline& rhs)
	{
		operator=(rhs);
	}

	EdgeSpline::EdgeSpline(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	EdgeSpline::EdgeSpline(bytes rhs)
	{
		Memory::Copy(&x, rhs, LengthOfWrite());
	}

	EdgeSpline::~EdgeSpline()
	{
	}

	EdgeSpline& EdgeSpline::operator=(const EdgeSpline& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	bool EdgeSpline::IsValid() const
	{
		return true;
	}

	const Point EdgeSpline::ValidPoint() const
	{
		return Point(x, y);
	}

	sint32 EdgeSpline::LengthOfWrite() const
	{
		return sizeof(EdgeSpline) + ((sblock) this) - ((sblock) &x);
	}

	void EdgeSpline::Write(uint08* ptr) const
	{
		Memory::Copy(ptr, &x, LengthOfWrite());
	}

	//////////////////////////////////////////////////
	EdgeTube::EdgeTube()
	{
		endmark = true;
		force = 0;
		lx = 0;
		ly = 0;
		rx = 0;
		ry = 0;
	}

	EdgeTube::EdgeTube(const EdgeTube& rhs)
	{
		operator=(rhs);
	}

	EdgeTube::EdgeTube(float force, float lx, float ly, float rx, float ry)
	{
		this->endmark = false;
		this->force = force;
		this->lx = lx;
		this->ly = ly;
		this->rx = rx;
		this->ry = ry;
	}

	EdgeTube::EdgeTube(bytes rhs)
	{
		Memory::Copy(&endmark, rhs, LengthOfWrite());
	}

	EdgeTube::~EdgeTube()
	{
	}

	EdgeTube& EdgeTube::operator=(const EdgeTube& rhs)
	{
		endmark = rhs.endmark;
		force = rhs.force;
		lx = rhs.lx;
		ly = rhs.ly;
		rx = rhs.rx;
		ry = rhs.ry;
		return *this;
	}

	bool EdgeTube::IsValid() const
	{
		return !endmark;
	}

	const Point EdgeTube::ValidPoint() const
	{
		return Point((lx + rx) / 2, (ly + ry) / 2);
	}

	sint32 EdgeTube::LengthOfWrite() const
	{
		return sizeof(EdgeTube) + ((sblock) this) - ((sblock) &endmark);
	}

	void EdgeTube::Write(uint08* ptr) const
	{
		Memory::Copy(ptr, &endmark, LengthOfWrite());
	}
}
