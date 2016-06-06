#pragma once
#include <platyvg.hpp>
#include "../api/adapter.hpp"
#include "../element/context.hpp"
#include "../element/coord.hpp"
#include "../element/edge.hpp"
#include "../element/matrix.hpp"
#include "../element/mesh.hpp"
#include "../element/point.hpp"
#include "../element/selector.hpp"
#include "../element/size.hpp"
#include "../element/zoom.hpp"

namespace PlatyVGCore
{
	//! \brief 좌표의 도형화
	class Shape
	{
	public:
		Shape();
		Shape(const Shape& rhs);
		virtual ~Shape();
		Shape& operator=(const Shape& rhs);

	public:
		virtual void Process(const Context& ctx, const Coords coods);
		virtual void Update(const Point& offset, const float scale, const float radius);
		virtual void Reset();

	public:
		bool IsSelected() const;
		void Select();
		void Select(const SelectBy selectby, Selector& selector, const MergeBy mergeby);
		bool SelectTest(const SelectBy selectby, Selector& selector);
		void Unselect();

	public:
		void ChangeRGB(const uint08 r, const uint08 g, const uint08 b);
		void ChangeAlpha(const uint08 alpha, const uint32 aqua);
		void ChangeAlpha(const uint08 alpha);
		void ChangeAqua(const uint32 aqua);
		void ChangeMatrix(const Point (&oldpoint)[2], const Point (&newpoint)[2], const Zoom zoom);

	public:
		void SetContext(const Context& ctx);
		const Context& GetContext() const;
		const Matrix& GetMatrix() const;
		// Edge
		virtual chars GetEdgeType() const;
		virtual sint32 GetEdgeCount() const;
		virtual const Edge* GetEdge(sint32 index) const;
		virtual void AddEdge(bytes edgebuffer);
		// Mesh
		virtual chars GetMeshType() const;
		virtual sint32 GetMeshCount() const;
		virtual const Mesh* GetMesh(sint32 index) const;

	protected:
		Context context;
		Matrix matrix;
		bool selected;
	};
	typedef Union<Shape> UnionShape;
	typedef Array<UnionShape> UnionShapes;

	//! \brief 좌표의 도형화(Spline)
	class ShapeSpline : public Shape
	{
	public:
		ShapeSpline();
		ShapeSpline(const ShapeSpline& rhs);
		virtual ~ShapeSpline();
		ShapeSpline& operator=(const ShapeSpline& rhs);
		void Init(const Context& ctx, const EdgeSplines& rhs);

	public:
		virtual void Process(const Context& ctx, const Coords coods);
		virtual void Update(const Point& offset, const float scale, const float radius);
		virtual void Reset();

	public:
		// Edge
		virtual chars GetEdgeType() const;
		virtual sint32 GetEdgeCount() const;
		virtual const Edge* GetEdge(sint32 index) const;
		virtual void AddEdge(bytes edgebuffer);
		// Mesh
		virtual chars GetMeshType() const;
		virtual sint32 GetMeshCount() const;
		virtual const Mesh* GetMesh(sint32 index) const;

	protected:
		EdgeSplines edges;
		MeshSplines meshes;
	};

	//! \brief 좌표의 도형화(Tube)
	class ShapeTube : public Shape
	{
	public:
		ShapeTube();
		ShapeTube(const ShapeTube& rhs);
		virtual ~ShapeTube();
		ShapeTube& operator=(const ShapeTube& rhs);
		void Init(const Context& ctx, const EdgeTubes& rhs);

	public:
		virtual void Process(const Context& ctx, const Coords coods);
		virtual void Update(const Point& offset, const float scale, const float radius);
		virtual void Reset();

	public:
		// Edge
		virtual chars GetEdgeType() const;
		virtual sint32 GetEdgeCount() const;
		virtual const Edge* GetEdge(sint32 index) const;
		virtual void AddEdge(bytes edgebuffer);
		// Mesh
		virtual chars GetMeshType() const;
		virtual sint32 GetMeshCount() const;
		virtual const Mesh* GetMesh(sint32 index) const;

	protected:
		EdgeTubes edges;
		MeshTubes meshes;
	};

	//! \brief 좌표의 도형화(TubeAqua)
	class ShapeTubeAqua : public ShapeTube
	{
	public:
		ShapeTubeAqua();
		ShapeTubeAqua(const ShapeTubeAqua& rhs);
		virtual ~ShapeTubeAqua();
		ShapeTubeAqua& operator=(const ShapeTubeAqua& rhs);

	protected:
		static void GetCubicSpline(const float VecX, const float VecY,
			const float VecXB, const float VecYB, Point& ResultB,
			const float VecXE, const float VecYE, Point& ResultE);
		static void Collect(MeshAquas& Collector, const MeshTube& Tube,
			const float Dense, const float ForceB, const float ForceE, const bool SortByReverse);

	public:
		virtual void Update(const Point& offset, const float scale, const float radius);
		virtual void Reset();

	public:
		// Mesh
		virtual chars GetMeshType() const;
		virtual sint32 GetMeshCount() const;
		virtual const Mesh* GetMesh(sint32 index) const;

	protected:
		MeshAquas meshes2;
	};
}
