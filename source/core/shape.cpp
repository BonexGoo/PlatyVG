#include <platyvg.hpp>
#include "shape.hpp"
#include "../element/point.hpp"
#include "../element/rect.hpp"

#define R0 (0.0)
#define R90 (R180 / 2)
#define R180 (3.14159265358979323846264338327950288419716939937511f)
#define R270 (R180 * 3 / 2)
#define R360 (R180 * 2)

namespace PlatyVGCore
{
	Shape::Shape()
	{
		selected = false;
	}

	Shape::Shape(const Shape& rhs)
	{
		operator=(rhs);
	}

	Shape::~Shape()
	{
	}

	Shape& Shape::operator=(const Shape& rhs)
	{
		context = rhs.context;
		matrix = rhs.matrix;
		selected = rhs.selected;
		return *this;
	}

	void Shape::Process(const Context& ctx, const Coords coods)
	{
	}

	void Shape::Update(const Point& offset, const float scale, const float radius)
	{
	}

	void Shape::Reset()
	{
		Unselect();
	}

	bool Shape::IsSelected() const
	{
		return selected;
	}

	void Shape::Select()
	{
		selected = true;
	}

	void Shape::Select(const SelectBy selectby, Selector& selector, const MergeBy mergeby)
	{
		switch(mergeby)
		{
		case MergeByPlus:
			if(!selected && SelectTest(selectby, selector))
				selected = true;
			break;
		case MergeByMinus:
			if(selected && !SelectTest(selectby, selector))
				selected = false;
			break;
		case MergeByReverse:
			if(SelectTest(selectby, selector))
				selected = !selected;
			break;
		}
	}

	bool Shape::SelectTest(const SelectBy selectby, Selector& selector)
	{
		switch(selectby)
		{
		case SelectByRect:
			if(selector.IsValid())
			{
				const Rect& SelectRect = selector.GetRect();
				Point OldPoint;
				bool HasOldPoint = false;
				for(sint32 i = 0, iend = GetEdgeCount(); i < iend; ++i)
				{
					const Edge& CurEdge = *GetEdge(i);
					if(!CurEdge.IsValid()) continue;
					const Point& CurPoint = CurEdge.ValidPoint();
					if(SelectRect.PtInRect(CurPoint))
						return true;
					if(HasOldPoint)
					{
						const bool TestLT = Point(SelectRect.l, SelectRect.t).ClockwiseTest(OldPoint, CurPoint);
						const bool TestRT = Point(SelectRect.r, SelectRect.t).ClockwiseTest(OldPoint, CurPoint);
						const bool TestLB = Point(SelectRect.l, SelectRect.b).ClockwiseTest(OldPoint, CurPoint);
						const bool TestRB = Point(SelectRect.r, SelectRect.b).ClockwiseTest(OldPoint, CurPoint);
						if(((SelectRect.l < OldPoint.x) != (SelectRect.l < CurPoint.x) && TestLT != TestLB)
						|| ((SelectRect.t < OldPoint.y) != (SelectRect.t < CurPoint.y) && TestLT != TestRT)
						|| ((SelectRect.r < OldPoint.x) != (SelectRect.r < CurPoint.x) && TestRT != TestRB)
						|| ((SelectRect.b < OldPoint.y) != (SelectRect.b < CurPoint.y) && TestLB != TestRB))
							return true;
					}
					OldPoint = CurPoint;
					HasOldPoint = true;
				}
			}
			break;
		case SelectByDrag:
			if(selector.IsValid())
			{
				const Points SelectPoints = selector.GetPoints();
				const Rects SelectRects = selector.GetRects();
				for(sint32 s = 0, send = SelectRects.Count(); s < send; ++s)
				{
					const Point& CurPoint1 = SelectPoints[s];
					const Point& CurPoint2 = SelectPoints[s + 1];
					const Rect& CurRect = SelectRects[s];
					Point OldPoint;
					bool HasOldPoint = false;
					for(sint32 i = 0, iend = GetEdgeCount(); i < iend; ++i)
					{
						const Edge& CurEdge = *GetEdge(i);
						if(!CurEdge.IsValid()) continue;
						const Point& CurPoint = CurEdge.ValidPoint();
						if(HasOldPoint && CurRect.ContactTest(Rect(OldPoint.x, OldPoint.y, CurPoint.x, CurPoint.y)))
						{
							const bool Test1 = CurPoint1.ClockwiseTest(OldPoint, CurPoint);
							const bool Test2 = CurPoint2.ClockwiseTest(OldPoint, CurPoint);
							const bool TestO = OldPoint.ClockwiseTest(CurPoint1, CurPoint2);
							const bool TestC = CurPoint.ClockwiseTest(CurPoint1, CurPoint2);
							if(Test1 != Test2 && TestO != TestC)
								return true;
						}
						OldPoint = CurPoint;
						HasOldPoint = true;
					}
				}
			}
			break;
		case SelectByArea:
			if(selector.IsValid())
			{
				sint32 YMin = 0;
				const Tables SelectTables = selector.GetTables(&YMin);
				Point OldPoint;
				bool HasOldPoint = false;
				for(sint32 i = 0, iend = GetEdgeCount(); i < iend; ++i)
				{
					const Edge& CurEdge = *GetEdge(i);
					if(!CurEdge.IsValid()) continue;
					const Point& CurPoint = CurEdge.ValidPoint();
					if(HasOldPoint)
					{
						Point BeginPoint = OldPoint;
						Point EndPoint = CurPoint;
						if(EndPoint.y < BeginPoint.y)
						{
							const Point Temp = BeginPoint;
							BeginPoint = EndPoint;
							EndPoint = Temp;
						}
						const sint32 YB = Math::Max(0, ((sint32) BeginPoint.y) - YMin);
						const sint32 YE = Math::Min(SelectTables.Count(), ((sint32) EndPoint.y) - YMin);
						for(sint32 y = YB; y < YE; ++y)
						{
							sint32 ClockwiseCount = 0;
							const Table& CurTable = SelectTables[y];
							for(sint32 x = 0, xend = CurTable.row.Count(); x < xend; ++x)
								if(Point(CurTable.row[x], YMin + y).ClockwiseTest(BeginPoint, EndPoint))
									++ClockwiseCount;
							if(ClockwiseCount & 1)
								return true;
						}
					}
                    else
                    {
                        const sint32 YBE = ((sint32) CurPoint.y) - YMin;
                        if(0 <= YBE && YBE < SelectTables.Count())
                        {
                            sint32 ClockwiseCount = 0;
                            const Table& CurTable = SelectTables[YBE];
                            for(sint32 x = 0, xend = CurTable.row.Count(); x < xend; ++x)
                                if(CurTable.row[x] < CurPoint.x)
                                    ++ClockwiseCount;
                            if(ClockwiseCount & 1)
								return true;
                        }
                    }
					OldPoint = CurPoint;
					HasOldPoint = true;
				}
			}
			break;
		}
		return false;
	}

	void Shape::Unselect()
	{
		selected = false;
	}

	void Shape::ChangeRGB(const uint08 r, const uint08 g, const uint08 b)
	{
		context.color.r = r;
		context.color.g = g;
		context.color.b = b;
	}

	void Shape::ChangeAlpha(const uint08 alpha)
	{
		context.color.a = alpha;
	}

	void Shape::ChangeAqua(const uint32 aqua)
	{
		context.color.aqua = aqua;
	}

	void Shape::ChangeMatrix(const Point (&oldpoint)[2], const Point (&newpoint)[2], const Zoom zoom)
	{
		const Point A1 = zoom.ToDocument(oldpoint[0]);
		const Point A2 = zoom.ToDocument(oldpoint[1]);
		const Point B1 = zoom.ToDocument(newpoint[0]);
		const Point B2 = zoom.ToDocument(newpoint[1]);
		matrix.AddByTouch(A1.x, A1.y, A2.x, A2.y, B1.x, B1.y, B2.x, B2.y);
	}

	void Shape::SetContext(const Context& ctx)
	{
		context = ctx;
	}

	const Context& Shape::GetContext() const
	{
		return context;
	}

	const Matrix& Shape::GetMatrix() const
	{
		return matrix;
	}

	chars Shape::GetEdgeType() const
	{
		return "";
	}

	sint32 Shape::GetEdgeCount() const
	{
		return 0;
	}

	const Edge* Shape::GetEdge(sint32 index) const
	{
		return nullptr;
	}

	void Shape::AddEdge(bytes edgebuffer)
	{
	}

	chars Shape::GetMeshType() const
	{
		return "";
	}

	sint32 Shape::GetMeshCount() const
	{
		return 0;
	}

	const Mesh* Shape::GetMesh(sint32 index) const
	{
		return nullptr;
	}

	//////////////////////////////////////////////////
	ShapeSpline::ShapeSpline()
	{
	}

	ShapeSpline::ShapeSpline(const ShapeSpline& rhs)
	{
		operator=(rhs);
	}

	ShapeSpline::~ShapeSpline()
	{
	}

	ShapeSpline& ShapeSpline::operator=(const ShapeSpline& rhs)
	{
		Shape::operator=(*((Shape*) &rhs));
		edges = rhs.edges;
		meshes = rhs.meshes;
		return *this;
	}

	void ShapeSpline::Init(const Context& ctx, const EdgeSplines& rhs)
	{
		context = ctx;
		edges = rhs;
	}

	void ShapeSpline::Process(const Context& ctx, const Coords coods)
	{
		EdgeSplines Collector;
		for(sint32 i = 0, iend = coods.Count(); i < iend; ++i)
			Collector.AtAdding() = EdgeSpline(coods[i].x, coods[i].y);
		context = ctx;
		edges = Collector;
	}

	void ShapeSpline::Update(const Point& offset, const float scale, const float radius)
	{
		MeshSplines Collector;
		for(int i = 0, iend = edges.Count(); i < iend; ++i)
			Collector.AtAdding() = MeshSpline(
				(edges[i].x + offset.x) * scale,
				(edges[i].y + offset.y) * scale);
		meshes = Collector;
	}

	void ShapeSpline::Reset()
	{
		Shape::Reset();
		edges.Clear();
		meshes.Clear();
	}

	chars ShapeSpline::GetEdgeType() const
	{
		return edges.Type();
	}

	sint32 ShapeSpline::GetEdgeCount() const
	{
		return edges.Count();
	}

	const Edge* ShapeSpline::GetEdge(sint32 index) const
	{
		return &edges[index];
	}

	void ShapeSpline::AddEdge(bytes edgebuffer)
	{
		edges.AtAdding() = EdgeSpline(edgebuffer);
	}

	chars ShapeSpline::GetMeshType() const
	{
		return meshes.Type();
	}

	sint32 ShapeSpline::GetMeshCount() const
	{
		return meshes.Count();
	}

	const Mesh* ShapeSpline::GetMesh(sint32 index) const
	{
		return &meshes[index];
	}

	//////////////////////////////////////////////////
	ShapeTube::ShapeTube()
	{
	}

	ShapeTube::ShapeTube(const ShapeTube& rhs)
	{
		operator=(rhs);
	}

	ShapeTube::~ShapeTube()
	{
	}

	ShapeTube& ShapeTube::operator=(const ShapeTube& rhs)
	{
		Shape::operator=(*((Shape*) &rhs));
		edges = rhs.edges;
		meshes = rhs.meshes;
		return *this;
	}

	void ShapeTube::Init(const Context& ctx, const EdgeTubes& rhs)
	{
		context = ctx;
		edges = rhs;
	}

	void ShapeTube::Process(const Context& ctx, const Coords coods)
	{
		if(coods.Count() < 1)
		{
			context = ctx;
			edges = EdgeTubes();
		}

		const float LimitRule = 0.5f;
		float PrevForce = coods[0].force;
		float PrevThick = ctx.thick * PrevForce;
		float CurrForce = PrevForce;
		float CurrThick = PrevThick;
		Point PrevPt = Point(coods[0].x, coods[0].y);
		Point CurrPt = PrevPt;
		float OldTLx = 0.0f, OldTLy = 0.0f, OldBLx = 0.0f, OldBLy = 0.0f;
		int CountFromMove = 0;

		EdgeTubes Collector;
		for(int i = 1, iend = coods.Count(); i < iend; ++i)
		{
			CurrPt = Point(coods[i].x, coods[i].y);
			CurrForce = coods[i].force;
			CurrThick = ctx.thick * CurrForce;
			const float xdist = CurrPt.x - PrevPt.x;
			const float ydist = CurrPt.y - PrevPt.y;
			const float dist = Math::Sqrt(xdist * xdist + ydist * ydist);
			if(PrevThick * LimitRule < dist)
			{
				// 끝선병합 판단1
				bool can_merge_endline = false;
				for(int j = i + 1; j < iend; ++j)
				{
					const Point NextPt = Point(coods[j].x, coods[j].y);
					const float xdist2 = NextPt.x - CurrPt.x;
					const float ydist2 = NextPt.y - CurrPt.y;
					const float dist2 = Math::Sqrt(xdist2 * xdist2 + ydist2 * ydist2);
					if(PrevThick * LimitRule < dist2)
					{
						can_merge_endline = false;
						break;
					}
					else can_merge_endline = true;
				}
				// 끝선병합 판단2
				if(can_merge_endline)
				{
					const Point EndPt = Point(coods[iend - 1].x, coods[iend - 1].y);
					const float xdist3 = EndPt.x - PrevPt.x;
					const float ydist3 = EndPt.y - PrevPt.y;
					const float dist3 = Math::Sqrt(xdist3 * xdist3 + ydist3 * ydist3);
					if(PrevThick * LimitRule < dist3)
					{
						i = iend - 2;
						continue;
					}
				}

				const float prev_direction = Math::Atan(PrevPt.x - CurrPt.x, PrevPt.y - CurrPt.y);
				const float rx = PrevThick * 0.5f * xdist / dist;
				const float ry = PrevThick * 0.5f * ydist / dist;
				if(CountFromMove == 0)
				{
					OldTLx = PrevPt.x + ry; OldTLy = PrevPt.y - rx;
					OldBLx = PrevPt.x - ry; OldBLy = PrevPt.y + rx;
					Collector.AtAdding();
					Collector.AtAdding() = EdgeTube(PrevForce, OldTLx, OldTLy, OldBLx, OldBLy);
					CountFromMove = 1;
				}
				else
				{
					const float ab_direction = Math::Atan(OldBLx - OldTLx, OldBLy - OldTLy);
					// prev방향기준 ab방향의 사이각
					float prev_to_ab = ab_direction - prev_direction;
					if(prev_to_ab < R0) prev_to_ab += R360;
					// 꼬임처리
					if(prev_to_ab < R180)
					{
						const float xSwap = OldTLx; OldTLx = OldBLx; OldBLx = xSwap;
						const float ySwap = OldTLy; OldTLy = OldBLy; OldBLy = ySwap;
						Collector.AtAdding();
						Collector.AtAdding() = EdgeTube(PrevForce, OldTLx, OldTLy, OldBLx, OldBLy);
						CountFromMove = 1;
					}
				}

				if(i < iend - 1)
				{
					const Point NextPt = Point(coods[i + 1].x, coods[i + 1].y);
					const float next_direction = Math::Atan(NextPt.x - CurrPt.x, NextPt.y - CurrPt.y);
					// prev방향기준 next방향의 사이각
					float prev_to_next = next_direction - prev_direction;
					if(prev_to_next < R0) prev_to_next += R360;
					// 사이각이 90도미만, 270도초과의 첨도를 가지면 꼬임판단
					if(prev_to_next < R90) prev_to_next += R180;
					else if(R270 < prev_to_next) prev_to_next -= R180;

					// 사이각의 삼각비로 기울기구간(-1/+1) 산정
					const float add_rate = (1 - Math::Cos(prev_to_next - R180)) * ((prev_to_next < R180)? -1 : 1);
					// 기울기량에 의한 현재 선분값 산출
					const float add_x = rx * add_rate, add_y = ry * add_rate;
					float CurTLx = CurrPt.x + ry + add_x, CurTLy = CurrPt.y - rx + add_y;
					float CurBLx = CurrPt.x - ry - add_x, CurBLy = CurrPt.y + rx - add_y;

					// 우평면 클리핑
					bool is_clipped_a = false, is_clipped_b = false;
					const float dist_x = OldBLx - OldTLx, dist_y = OldBLy - OldTLy;
					if((CurTLx - OldTLx) * dist_y < (CurTLy - OldTLy) * dist_x)
					{
						is_clipped_a = true;
						CurTLx = OldTLx;
						CurTLy = OldTLy;
					}
					if((CurBLx - OldTLx) * dist_y < (CurBLy - OldTLy) * dist_x)
					{
						is_clipped_b = true;
						CurBLx = OldBLx;
						CurBLy = OldBLy;
					}

					// 간격보정
					const float end_dist_x = CurTLx - CurBLx, end_dist_y = CurTLy - CurBLy;
					const float end_dist = Math::Sqrt(end_dist_x * end_dist_x + end_dist_y * end_dist_y);
					const float end_add_x = end_dist_x * PrevThick / end_dist;
					const float end_add_y = end_dist_y * PrevThick / end_dist;
					if(is_clipped_a && is_clipped_b)
					{
						const float middle_x = (CurTLx + CurBLx) * 0.5f;
						const float middle_y = (CurTLy + CurBLy) * 0.5f;
						CurTLx = middle_x + end_add_x * 0.5f;
						CurTLy = middle_y + end_add_y * 0.5f;
						CurBLx = middle_x - end_add_x * 0.5f;
						CurBLy = middle_y - end_add_y * 0.5f;
					}
					if(is_clipped_a)
					{
						CurBLx = CurTLx - end_add_x;
						CurBLy = CurTLy - end_add_y;
					}
					else
					{
						CurTLx = CurBLx + end_add_x;
						CurTLy = CurBLy + end_add_y;
					}
					Collector.AtAdding() = EdgeTube(PrevForce, CurTLx, CurTLy, CurBLx, CurBLy);

					// 클리핑관련 재설정
					PrevPt = Point((CurTLx + CurBLx) * 0.5f, (CurTLy + CurBLy) * 0.5f);
					PrevForce = CurrForce;
					PrevThick = CurrThick;
					OldTLx = CurTLx; OldTLy = CurTLy;
					OldBLx = CurBLx; OldBLy = CurBLy;
					CountFromMove++;
				}
				else Collector.AtAdding() = EdgeTube(PrevForce, CurrPt.x + ry, CurrPt.y - rx, CurrPt.x - ry, CurrPt.y + rx);
			}
		}

		// 끝점처리
		if(CountFromMove == 0)
		{
			const float xdist = CurrPt.x - PrevPt.x;
			const float ydist = CurrPt.y - PrevPt.y;
			const float thick = Math::MaxF(PrevThick * LimitRule / 2, Math::Sqrt(xdist * xdist + ydist * ydist));
			const float x = (CurrPt.x + PrevPt.x) / 2;
			const float y = (CurrPt.y + PrevPt.y) / 2;
			Collector.AtAdding();
			Collector.AtAdding() = EdgeTube(PrevForce, x - thick, y - thick, x - thick, y + thick);
			Collector.AtAdding() = EdgeTube(PrevForce, x + thick, y - thick, x + thick, y + thick);
		}

		// 마감처리
		Collector.AtAdding();
		context = ctx;
		edges = Collector;
	}

	void ShapeTube::Update(const Point& offset, const float scale, const float radius)
	{
		float Lx[4], Ly[4], Rx[4], Ry[4];
		int EdgeCount = 0;

		MeshTubes Collector;
		for(int i = 0, iend = edges.Count(); i < iend; ++i)
		{
			const int Index0 = (EdgeCount + 0) & 3;
			const int Index1 = (EdgeCount + 3) & 3;
			const int Index2 = (EdgeCount + 2) & 3;
			const int Index3 = (EdgeCount + 1) & 3;
			if(edges[i].endmark)
			{
				if(2 < EdgeCount)
					Collector.AtAdding() = MeshTube(true, // End
						Rx[Index3], Rx[Index2], Rx[Index1], 0.0f,
						Ry[Index3], Ry[Index2], Ry[Index1], 0.0f,
						Lx[Index3], Lx[Index2], Lx[Index1], 0.0f,
						Ly[Index3], Ly[Index2], Ly[Index1], 0.0f);
				else if(EdgeCount == 2)
				{
					const float Lxm = (Lx[Index1] + Lx[Index2]) * 0.5f;
					const float Lym = (Ly[Index1] + Ly[Index2]) * 0.5f;
					const float Rxm = (Rx[Index1] + Rx[Index2]) * 0.5f;
					const float Rym = (Ry[Index1] + Ry[Index2]) * 0.5f;
					Collector.AtAdding() = MeshTube(true, // Begin
						Lx[Index1], Lxm, Lx[Index2], 0.0f,
						Ly[Index1], Lym, Ly[Index2], 0.0f,
						Rx[Index1], Rxm, Rx[Index2], 0.0f,
						Ry[Index1], Rym, Ry[Index2], 0.0f);
					Collector.AtAdding() = MeshTube(true, // End
						Rx[Index2], Rxm, Rx[Index1], 0.0f,
						Ry[Index2], Rym, Ry[Index1], 0.0f,
						Lx[Index2], Lxm, Lx[Index1], 0.0f,
						Ly[Index2], Lym, Ly[Index1], 0.0f);
				}
				EdgeCount = 0;
			}
			else
			{
				Lx[Index0] = (edges[i].lx + offset.x) * scale;
				Ly[Index0] = (edges[i].ly + offset.y) * scale;
				Rx[Index0] = (edges[i].rx + offset.x) * scale;
				Ry[Index0] = (edges[i].ry + offset.y) * scale;
				if(2 < ++EdgeCount)
					Collector.AtAdding() = MeshTube(EdgeCount < 4, // Begin or middle
						Lx[Index0], Lx[Index1], Lx[Index2], Lx[Index3],
						Ly[Index0], Ly[Index1], Ly[Index2], Ly[Index3],
						Rx[Index0], Rx[Index1], Rx[Index2], Rx[Index3],
						Ry[Index0], Ry[Index1], Ry[Index2], Ry[Index3]);
			}
		}
		meshes = Collector;
	}

	void ShapeTube::Reset()
	{
		Shape::Reset();
		edges.Clear();
		meshes.Clear();
	}

	chars ShapeTube::GetEdgeType() const
	{
		return edges.Type();
	}

	sint32 ShapeTube::GetEdgeCount() const
	{
		return edges.Count();
	}

	const Edge* ShapeTube::GetEdge(sint32 index) const
	{
		return &edges[index];
	}

	void ShapeTube::AddEdge(bytes edgebuffer)
	{
		edges.AtAdding() = EdgeTube(edgebuffer);
	}

	chars ShapeTube::GetMeshType() const
	{
		return meshes.Type();
	}

	sint32 ShapeTube::GetMeshCount() const
	{
		return meshes.Count();
	}

	const Mesh* ShapeTube::GetMesh(sint32 index) const
	{
		return &meshes[index];
	}

	//////////////////////////////////////////////////
	ShapeTubeAqua::ShapeTubeAqua()
	{
	}

	ShapeTubeAqua::ShapeTubeAqua(const ShapeTubeAqua& rhs)
	{
		operator=(rhs);
	}

	ShapeTubeAqua::~ShapeTubeAqua()
	{
	}

	ShapeTubeAqua& ShapeTubeAqua::operator=(const ShapeTubeAqua& rhs)
	{
		ShapeTube::operator=(*((ShapeTube*) &rhs));
		meshes2 = rhs.meshes2;
		return *this;
	}

	void ShapeTubeAqua::GetCubicSpline(const float VecX, const float VecY,
		const float VecXB, const float VecYB, Point& ResultB,
		const float VecXE, const float VecYE, Point& ResultE)
	{
		const float dist = Math::Sqrt(VecX * VecX + VecY * VecY);
		if(0 < dist)
		{
			const float distb = Math::Sqrt(VecXB * VecXB + VecYB * VecYB);
			if(0 < distb)
			{
				const float k = distb / dist;
				const float m = 2 * k * (1 + k);
				const float kk = k * k;
				ResultB.x = 3 * (VecXB + kk * VecX) / m;
				ResultB.y = 3 * (VecYB + kk * VecY) / m;
			}
			const float diste = Math::Sqrt(VecXE * VecXE + VecYE * VecYE);
			if(0 < diste)
			{
				const float k = dist / diste;
				const float m = 2 * k * (1 + k);
				const float kk = k * k;
				ResultE.x = k * 3 * (VecX + kk * VecXE) / m;
				ResultE.y = k * 3 * (VecY + kk * VecYE) / m;
			}
		}
	}

	void ShapeTubeAqua::Collect(MeshAquas& Collector, const MeshTube& Tube,
		const float Dense, const float ForceB, const float ForceE, const bool SortByReverse)
	{
		Point LB, LE;
		const float LVecX = Tube.lx[2] - Tube.lx[1];
		const float LVecY = Tube.ly[2] - Tube.ly[1];
		const float LVecXB = Tube.lx[1] - Tube.lx[0];
		const float LVecYB = Tube.ly[1] - Tube.ly[0];
		const float LVecXE = Tube.lx[3] - Tube.lx[2];
		const float LVecYE = Tube.ly[3] - Tube.ly[2];
		GetCubicSpline(LVecX, LVecY, LVecXB, LVecYB, LB, LVecXE, LVecYE, LE);
		Point RB, RE;
		const float RVecX = Tube.rx[2] - Tube.rx[1];
		const float RVecY = Tube.ry[2] - Tube.ry[1];
		const float RVecXB = Tube.rx[1] - Tube.rx[0];
		const float RVecYB = Tube.ry[1] - Tube.ry[0];
		const float RVecXE = Tube.rx[3] - Tube.rx[2];
		const float RVecYE = Tube.ry[3] - Tube.ry[2];
		GetCubicSpline(RVecX, RVecY, RVecXB, RVecYB, RB, RVecXE, RVecYE, RE);

		const float Force = ForceE - ForceB;
		const float LMax = Math::MaxF(Math::AbsF(LVecX), Math::AbsF(LVecY));
		const float RMax = Math::MaxF(Math::AbsF(RVecX), Math::AbsF(RVecY));
		const sint32 Count = Math::Max(1, (sint32) (Math::MaxF(LMax, RMax) * Dense + 0.5f));
		const sint32 ibegin = (SortByReverse)? Count : 0;
		const sint32 iend = (SortByReverse)? -1 : Count + 1;
		const sint32 istep = (SortByReverse)? -1 : 1;
		for(sint32 i = ibegin; i != iend; i += istep)
		{
			const float t = i / (float) Count;
			const float tt = t * t;
			const float r = t - 1;
			const float av = tt * (3 - 2 * t);
			const float bv = t * r * r;
			const float cv = tt * r;
			Collector.AtAdding() = MeshAqua(ForceB + Force * t,
				Tube.lx[1] + LVecX * av + LB.x * bv + LE.x * cv,
				Tube.ly[1] + LVecY * av + LB.y * bv + LE.y * cv,
				Tube.rx[1] + RVecX * av + RB.x * bv + RE.x * cv,
				Tube.ry[1] + RVecY * av + RB.y * bv + RE.y * cv);
		}
	}

	void ShapeTubeAqua::Update(const Point& offset, const float scale, const float radius)
	{
		float Fr[4], Lx[4], Ly[4], Rx[4], Ry[4], Dr[4];
		sint32 EdgeCount = 0;
		const float BaseDense = scale / 5;
		const float BaseRadius = radius * radius;

		MeshAquas Collector;
		for(sint32 i = 0, iend = edges.Count(); i < iend; ++i)
		{
			const EdgeTube& CurEdge = edges[i];
			const sint32 Index0 = (EdgeCount + 0) & 3;
			const sint32 Index1 = (EdgeCount + 3) & 3;
			const sint32 Index2 = (EdgeCount + 2) & 3;
			const sint32 Index3 = (EdgeCount + 1) & 3;
			if(CurEdge.endmark)
			{
				if(2 < EdgeCount)
					Collect(Collector, MeshTube(true, // End
						Rx[Index3], Rx[Index2], Rx[Index1], 0.0f,
						Ry[Index3], Ry[Index2], Ry[Index1], 0.0f,
						Lx[Index3], Lx[Index2], Lx[Index1], 0.0f,
						Ly[Index3], Ly[Index2], Ly[Index1], 0.0f),
						Math::MaxF(Dr[Index2], Dr[Index1]),
						Fr[Index2], Fr[Index1], false);
				else if(EdgeCount == 2)
				{
					const float Frm = (Fr[Index1] + Fr[Index2]) * 0.5f;
					const float Lxm = (Lx[Index1] + Lx[Index2]) * 0.5f;
					const float Lym = (Ly[Index1] + Ly[Index2]) * 0.5f;
					const float Rxm = (Rx[Index1] + Rx[Index2]) * 0.5f;
					const float Rym = (Ry[Index1] + Ry[Index2]) * 0.5f;
					const float Drm = (Dr[Index1] + Dr[Index2]) * 0.5f;
					Collect(Collector, MeshTube(true, // Begin
						Lx[Index1], Lxm, Lx[Index2], 0.0f,
						Ly[Index1], Lym, Ly[Index2], 0.0f,
						Rx[Index1], Rxm, Rx[Index2], 0.0f,
						Ry[Index1], Rym, Ry[Index2], 0.0f),
						Math::MaxF(Drm, Dr[Index2]),
						Frm, Fr[Index2], true);
					Collect(Collector, MeshTube(true, // End
						Rx[Index2], Rxm, Rx[Index1], 0.0f,
						Ry[Index2], Rym, Ry[Index1], 0.0f,
						Lx[Index2], Lxm, Lx[Index1], 0.0f,
						Ly[Index2], Lym, Ly[Index1], 0.0f),
						Math::MaxF(Drm, Dr[Index1]),
						Frm, Fr[Index1], false);
				}
				EdgeCount = 0;
			}
			else
			{
				Fr[Index0] = CurEdge.force;
				Lx[Index0] = CurEdge.lx;
				Ly[Index0] = CurEdge.ly;
				Rx[Index0] = CurEdge.rx;
				Ry[Index0] = CurEdge.ry;
				const float DistX = ((CurEdge.lx + CurEdge.rx) / 2 + offset.x) * scale;
				const float DistY = ((CurEdge.ly + CurEdge.ry) / 2 + offset.y) * scale;
				const float Dist = DistX * DistX + DistY * DistY;
				if(0 < Dist) Dr[Index0] = BaseDense * Math::MinF(BaseRadius / Dist, 1);
				else Dr[Index0] = BaseDense;
				if(2 < ++EdgeCount)
					Collect(Collector, MeshTube(EdgeCount < 4, // Begin or middle
						Lx[Index0], Lx[Index1], Lx[Index2], Lx[Index3],
						Ly[Index0], Ly[Index1], Ly[Index2], Ly[Index3],
						Rx[Index0], Rx[Index1], Rx[Index2], Rx[Index3],
						Ry[Index0], Ry[Index1], Ry[Index2], Ry[Index3]),
						Math::MaxF(Dr[Index1], Dr[Index2]),
						Fr[Index1], Fr[Index2], true);
			}
		}
		meshes2 = Collector;
	}

	/*void ShapeTubeAqua::Update(const Point& offset, const float scale, const float radius)
	{
		float Lx[4], Ly[4], Rx[4], Ry[4], Dr[4], Fr[4];
		sint32 EdgeCount = 0;
		const float BaseDense = scale / 5;
		const float BaseRadius = radius * radius;

		MeshAquas Collector;
		for(sint32 i = 0, iend = edges.Count(); i < iend; ++i)
		{
			const EdgeTube& CurEdge = edges[i];
			const sint32 Index0 = (EdgeCount + 0) & 3;
			const sint32 Index1 = (EdgeCount + 3) & 3;
			const sint32 Index2 = (EdgeCount + 2) & 3;
			const sint32 Index3 = (EdgeCount + 1) & 3;
			if(CurEdge.endmark)
			{
				if(2 < EdgeCount)
				{
					const float Ex0 = (Lx[Index1] + Rx[Index1]) * 0.5f;
					const float Ey0 = (Ly[Index1] + Ry[Index1]) * 0.5f;
					const float Mx0 = Ex0 - (Ly[Index1] - Ey0) * 0.5f;
					const float My0 = Ey0 + (Lx[Index1] - Ex0) * 0.5f;
					Collect(Collector, MeshTube(false, // Middle
						Mx0, Lx[Index1], Lx[Index2], Lx[Index3],
						My0, Ly[Index1], Ly[Index2], Ly[Index3],
						Mx0, Rx[Index1], Rx[Index2], Rx[Index3],
						My0, Ry[Index1], Ry[Index2], Ry[Index3]),
						Math::MaxF(Dr[Index1], Dr[Index2]),
						Fr[Index1], Fr[Index2], true);
					Collect(Collector, MeshTube(false, // End
						Rx[Index1], Mx0, Lx[Index1], Lx[Index2],
						Ry[Index1], My0, Ly[Index1], Ly[Index2],
						Lx[Index1], Mx0, Rx[Index1], Rx[Index2],
						Ly[Index1], My0, Ry[Index1], Ry[Index2]),
						Dr[Index1], Fr[Index1], Fr[Index1], true);
				}
				else if(EdgeCount == 2)
				{
					const float Ex3 = (Lx[Index2] + Rx[Index2]) * 0.5f;
					const float Ey3 = (Ly[Index2] + Ry[Index2]) * 0.5f;
					const float Mx3 = Ex3 + (Ly[Index2] - Ey3) * 0.5f;
					const float My3 = Ey3 - (Lx[Index2] - Ex3) * 0.5f;
					const float Ex0 = (Lx[Index1] + Rx[Index1]) * 0.5f;
					const float Ey0 = (Ly[Index1] + Ry[Index1]) * 0.5f;
					const float Mx0 = Ex0 - (Ly[Index1] - Ey0) * 0.5f;
					const float My0 = Ey0 + (Lx[Index1] - Ex0) * 0.5f;
					const float Drm = (Dr[Index1] + Dr[Index2]) * 0.5f;
					const float Frm = (Fr[Index1] + Fr[Index2]) * 0.5f;
					Collect(Collector, MeshTube(false, // Begin
						Lx[Index1], Lx[Index2], Mx3, Rx[Index2],
						Ly[Index1], Ly[Index2], My3, Ry[Index2],
						Rx[Index1], Rx[Index2], Mx3, Lx[Index2],
						Ry[Index1], Ry[Index2], My3, Ly[Index2]),
						Dr[Index2], Frm, Frm, true);
					Collect(Collector, MeshTube(false, // Middle
						Mx0, Lx[Index1], Lx[Index2], Mx3,
						My0, Ly[Index1], Ly[Index2], My3,
						Mx0, Rx[Index1], Rx[Index2], Mx3,
						My0, Ry[Index1], Ry[Index2], My3),
						Drm, Frm, Frm, true);
					Collect(Collector, MeshTube(false, // End
						Rx[Index1], Mx0, Lx[Index1], Lx[Index2],
						Ry[Index1], My0, Ly[Index1], Ly[Index2],
						Lx[Index1], Mx0, Rx[Index1], Rx[Index2],
						Ly[Index1], My0, Ry[Index1], Ry[Index2]),
						Dr[Index1], Frm, Frm, true);
				}
				EdgeCount = 0;
			}
			else
			{
				const Point L = Point(CurEdge.lx, CurEdge.ly) * matrix;
				const Point R = Point(CurEdge.rx, CurEdge.ry) * matrix;				
				Lx[Index0] = L.x;
				Ly[Index0] = L.y;
				Rx[Index0] = R.x;
				Ry[Index0] = R.y;
				const float DistX = ((L.x + R.x) / 2 + offset.x) * scale;
				const float DistY = ((L.y + R.y) / 2 + offset.y) * scale;
				const float Dist = DistX * DistX + DistY * DistY;
				if(0 < Dist) Dr[Index0] = BaseDense * Math::MinF(BaseRadius / Dist, 1);
				else Dr[Index0] = BaseDense;
				Fr[Index0] = CurEdge.force;

				if(2 < ++EdgeCount)
				{
					if(EdgeCount < 4)
					{
						const float Ex3 = (Lx[Index2] + Rx[Index2]) * 0.5f;
						const float Ey3 = (Ly[Index2] + Ry[Index2]) * 0.5f;
						const float Mx3 = Ex3 + (Ly[Index2] - Ey3) * 0.5f;
						const float My3 = Ey3 - (Lx[Index2] - Ex3) * 0.5f;
						Collect(Collector, MeshTube(false, // Begin
							Lx[Index1], Lx[Index2], Mx3, Rx[Index2],
							Ly[Index1], Ly[Index2], My3, Ry[Index2],
							Rx[Index1], Rx[Index2], Mx3, Lx[Index2],
							Ry[Index1], Ry[Index2], My3, Ly[Index2]),
							Dr[Index2], Fr[Index2], Fr[Index2], true);
						Collect(Collector, MeshTube(false, // Middle
							Lx[Index0], Lx[Index1], Lx[Index2], Mx3,
							Ly[Index0], Ly[Index1], Ly[Index2], My3,
							Rx[Index0], Rx[Index1], Rx[Index2], Mx3,
							Ry[Index0], Ry[Index1], Ry[Index2], My3),
							Math::MaxF(Dr[Index1], Dr[Index2]),
							Fr[Index1], Fr[Index2], true);
					}
					else
					{
						Collect(Collector, MeshTube(false, // Middle
							Lx[Index0], Lx[Index1], Lx[Index2], Lx[Index3],
							Ly[Index0], Ly[Index1], Ly[Index2], Ly[Index3],
							Rx[Index0], Rx[Index1], Rx[Index2], Rx[Index3],
							Ry[Index0], Ry[Index1], Ry[Index2], Ry[Index3]),
							Math::MaxF(Dr[Index1], Dr[Index2]),
							Fr[Index1], Fr[Index2], true);
					}
				}
			}
		}
		meshes2 = Collector;
	}*/

	void ShapeTubeAqua::Reset()
	{
		ShapeTube::Reset();
		meshes2.Clear();
	}

	chars ShapeTubeAqua::GetMeshType() const
	{
		return meshes2.Type();
	}

	sint32 ShapeTubeAqua::GetMeshCount() const
	{
		return meshes2.Count();
	}

	const Mesh* ShapeTubeAqua::GetMesh(sint32 index) const
	{
		return &meshes2[index];
	}
}
