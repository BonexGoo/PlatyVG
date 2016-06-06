#pragma once
#include <platyvg.hpp>

namespace PlatyVGElement
{
	//! \brief 메시좌표
	class Mesh
	{
	public:
		Mesh();
		Mesh(const Mesh& rhs);
		~Mesh();
		Mesh& operator=(const Mesh& rhs);
	};

	//! \brief 메시좌표(Spline)
	class MeshSpline : public Mesh
	{
	public:
		MeshSpline();
		MeshSpline(const MeshSpline& rhs);
		MeshSpline(float x, float y);
		~MeshSpline();
		MeshSpline& operator=(const MeshSpline& rhs);

	public:
		float x;
		float y;
	};
	typedef Array<MeshSpline, true> MeshSplines;

	//! \brief 메시좌표(Tube)
	class MeshTube : public Mesh
	{
	public:
		MeshTube();
		MeshTube(const MeshTube& rhs);
		MeshTube(const bool cap,
			const float lx0, const float lx1, const float lx2, const float lx3,
			const float ly0, const float ly1, const float ly2, const float ly3,
			const float rx0, const float rx1, const float rx2, const float rx3,
			const float ry0, const float ry1, const float ry2, const float ry3);
		~MeshTube();
		MeshTube& operator=(const MeshTube& rhs);

	public:
		float lx[4];
		float ly[4];
		float rx[4];
		float ry[4];
	};
	typedef Array<MeshTube, true> MeshTubes;

	//! \brief 메시좌표(Aqua)
	class MeshAqua : public Mesh
	{
	public:
		MeshAqua();
		MeshAqua(const MeshAqua& rhs);
		MeshAqua(const float force, const float lx, const float ly, const float rx, const float ry);
		~MeshAqua();
		MeshAqua& operator=(const MeshAqua& rhs);

	public:
		float force;
		float lx;
		float ly;
		float rx;
		float ry;
	};
	typedef Array<MeshAqua, true> MeshAquas;
}
