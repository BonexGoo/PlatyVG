#include <platyvg.hpp>
#include "mesh.hpp"

namespace PlatyVGElement
{
	Mesh::Mesh()
	{
	}

	Mesh::Mesh(const Mesh& rhs)
	{
		operator=(rhs);
	}

	Mesh::~Mesh()
	{
	}

	Mesh& Mesh::operator=(const Mesh& rhs)
	{
		return *this;
	}

	//////////////////////////////////////////////////
	MeshSpline::MeshSpline()
	{
		x = 0;
		y = 0;
	}

	MeshSpline::MeshSpline(const MeshSpline& rhs)
	{
		operator=(rhs);
	}

	MeshSpline::MeshSpline(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	MeshSpline::~MeshSpline()
	{
	}

	MeshSpline& MeshSpline::operator=(const MeshSpline& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	//////////////////////////////////////////////////
	MeshTube::MeshTube()
	{
		lx[0] = lx[1] = lx[2] = lx[3] = 0;
		ly[0] = ly[1] = ly[2] = ly[3] = 0;
		rx[0] = rx[1] = rx[2] = rx[3] = 0;
		ry[0] = ry[1] = ry[2] = ry[3] = 0;
	}

	MeshTube::MeshTube(const MeshTube& rhs)
	{
		operator=(rhs);
	}

	MeshTube::MeshTube(const bool cap,
		const float lx0, const float lx1, const float lx2, const float lx3,
		const float ly0, const float ly1, const float ly2, const float ly3,
		const float rx0, const float rx1, const float rx2, const float rx3,
		const float ry0, const float ry1, const float ry2, const float ry3)
	{
		if(cap)
		{
			float ox, oy;
			if(lx2 == lx1 && ly2 == ly1)
			{
				ox = rx2;
				oy = ry2;
			}
			else if(rx2 == rx1 && ry2 == ry1)
			{
				ox = lx2;
				oy = ly2;
			}
			else
			{
				ox = (lx2 + rx2) * 0.5f;
				oy = (ly2 + ry2) * 0.5f;
			}
			lx[0] = lx0; lx[1] = lx1; lx[2] = ox; lx[3] = rx1;
			ly[0] = ly0; ly[1] = ly1; ly[2] = oy; ly[3] = ry1;
			rx[0] = rx0; rx[1] = rx1; rx[2] = ox; rx[3] = lx1;
			ry[0] = ry0; ry[1] = ry1; ry[2] = oy; ry[3] = ly1;
		}
		else
		{
			lx[0] = lx0; lx[1] = lx1; lx[2] = lx2; lx[3] = lx3;
			ly[0] = ly0; ly[1] = ly1; ly[2] = ly2; ly[3] = ly3;
			rx[0] = rx0; rx[1] = rx1; rx[2] = rx2; rx[3] = rx3;
			ry[0] = ry0; ry[1] = ry1; ry[2] = ry2; ry[3] = ry3;
		}
	}

	MeshTube::~MeshTube()
	{
	}

	MeshTube& MeshTube::operator=(const MeshTube& rhs)
	{
		lx[0] = rhs.lx[0]; lx[1] = rhs.lx[1]; lx[2] = rhs.lx[2]; lx[3] = rhs.lx[3];
		ly[0] = rhs.ly[0]; ly[1] = rhs.ly[1]; ly[2] = rhs.ly[2]; ly[3] = rhs.ly[3];
		rx[0] = rhs.rx[0]; rx[1] = rhs.rx[1]; rx[2] = rhs.rx[2]; rx[3] = rhs.rx[3];
		ry[0] = rhs.ry[0]; ry[1] = rhs.ry[1]; ry[2] = rhs.ry[2]; ry[3] = rhs.ry[3];
		return *this;
	}

	//////////////////////////////////////////////////
	MeshAqua::MeshAqua()
	{
		force = 0;
		lx = 0;
		ly = 0;
		rx = 0;
		ry = 0;
	}

	MeshAqua::MeshAqua(const MeshAqua& rhs)
	{
		operator=(rhs);
	}

	MeshAqua::MeshAqua(const float force, const float lx, const float ly, const float rx, const float ry)
	{
		this->force = force;
		this->lx = lx;
		this->ly = ly;
		this->rx = rx;
		this->ry = ry;
	}

	MeshAqua::~MeshAqua()
	{
	}

	MeshAqua& MeshAqua::operator=(const MeshAqua& rhs)
	{
		force = rhs.force;
		lx = rhs.lx;
		ly = rhs.ly;
		rx = rhs.rx;
		ry = rhs.ry;
		return *this;
	}
}
