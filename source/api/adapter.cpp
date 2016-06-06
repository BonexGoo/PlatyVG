#include <platyvg.hpp>
#include "adapter.hpp"
#include "../core/view.hpp"

#define PROCLOOP64(CODE) { \
	case 0x00: CODE case 0x01: CODE case 0x02: CODE case 0x03: CODE \
	case 0x04: CODE case 0x05: CODE case 0x06: CODE case 0x07: CODE \
	case 0x08: CODE case 0x09: CODE case 0x0A: CODE case 0x0B: CODE \
	case 0x0C: CODE case 0x0D: CODE case 0x0E: CODE case 0x0F: CODE \
	case 0x10: CODE case 0x11: CODE case 0x12: CODE case 0x13: CODE \
	case 0x14: CODE case 0x15: CODE case 0x16: CODE case 0x17: CODE \
	case 0x18: CODE case 0x19: CODE case 0x1A: CODE case 0x1B: CODE \
	case 0x1C: CODE case 0x1D: CODE case 0x1E: CODE case 0x1F: CODE \
	case 0x20: CODE case 0x21: CODE case 0x22: CODE case 0x23: CODE \
	case 0x24: CODE case 0x25: CODE case 0x26: CODE case 0x27: CODE \
	case 0x28: CODE case 0x29: CODE case 0x2A: CODE case 0x2B: CODE \
	case 0x2C: CODE case 0x2D: CODE case 0x2E: CODE case 0x2F: CODE \
	case 0x30: CODE case 0x31: CODE case 0x32: CODE case 0x33: CODE \
	case 0x34: CODE case 0x35: CODE case 0x36: CODE case 0x37: CODE \
	case 0x38: CODE case 0x39: CODE case 0x3A: CODE case 0x3B: CODE \
	case 0x3C: CODE case 0x3D: CODE case 0x3E: CODE case 0x3F: CODE}

template<typename PROC>
static bool PolyProc(const sint32 x0, const sint32 y0, const sint32 x1, const sint32 y1, const sint32 x2, const sint32 y2, PROC proc)
{
	PX_ASSERT("파라미터오류", y0 <= y1 && y1 <= y2);
	const sint32 x0f = x0 << 16;
	const sint32 x1f = x1 << 16;
	const sint32 x2f = x2 << 16;

	if(y0 == y2)
		return false; // ━
	else if(y0 == y1)
	{
		const sint32 v02f = (x2f - x0f) / (y2 - y0);
		const sint32 v12f = (x2f - x1f) / (y2 - y1);
		if(x0 < x1) proc(v02f, v12f, x0f, x1f, y0, y2, false); // ▼
		else if(x0 > x1) proc(v12f, v02f, x1f, x0f, y0, y2, false); // ▼
		else return false; // ┃
	}
	else if(y1 == y2)
	{
		const sint32 v01f = (x1f - x0f) / (y1 - y0);
		const sint32 v02f = (x2f - x0f) / (y2 - y0);
		if(x1 < x2) proc(v01f, v02f, x0f, x0f, y0, y1, false); // ▲
		else if(x1 > x2) proc(v02f, v01f, x0f, x0f, y0, y1, false); // ▲
		else return false; // ┃
	}
	else
	{
		const sint32 v01f = (x1f - x0f) / (y1 - y0);
		const sint32 v02f = (x2f - x0f) / (y2 - y0);
		const sint32 v12f = (x2f - x1f) / (y2 - y1);
		if(v01f < v02f) // ◀
		{
			const sint32 tmp =
			proc(v01f, v02f, x0f, x0f, y0, y1, true);
			proc(v12f, v02f, x1f, tmp, y1, y2, true);
		}
		else if(v01f > v02f) // ▶
		{
			const sint32 tmp =
			proc(v02f, v01f, x0f, x0f, y0, y1, false);
			proc(v02f, v12f, tmp, x1f, y1, y2, false);
		}
		else return false; // ┃
	}
	return true;
}

template<typename PROC>
static bool PolyDraw(const sint32 xa, const sint32 ya, const sint32 xb, const sint32 yb, const sint32 xc, const sint32 yc, PROC proc)
{
	if(ya <= yb && ya <= yc)
	{
		if(yb <= yc)
			return PolyProc(xa, ya, xb, yb, xc, yc, proc);
		return PolyProc(xa, ya, xc, yc, xb, yb, proc);
	}

	if(yb <= ya && yb <= yc)
	{
		if(ya <= yc)
			return PolyProc(xb, yb, xa, ya, xc, yc, proc);
		return PolyProc(xb, yb, xc, yc, xa, ya, proc);
	}

	if(ya <= yb)
		return PolyProc(xc, yc, xa, ya, xb, yb, proc);
	return PolyProc(xc, yc, xb, yb, xa, ya, proc);
}

static inline const uint32 GetOpacityR(const uint32 value, const uint32 opacity)
{return (value * opacity / 0xFF) << 16;}

static inline const uint32 GetOpacityG(const uint32 value, const uint32 opacity)
{return (value * opacity / 0xFF) << 8;}

static inline const uint32 GetOpacityB(const uint32 value, const uint32 opacity)
{return (value * opacity / 0xFF) << 0;}

template<sint32 SET, typename FUNC>
static const uint32* GetDstOpacityTable(const uint32 opacity, FUNC func)
{
	static uint32 OpacityTable[256 * 256];
	static bool NeedInitialize = true;
	if(NeedInitialize)
	{
		NeedInitialize = false;
		for(uint32 opa = 0; opa < 256; ++opa)
		for(uint32 val = 0; val < 256; ++val)
			OpacityTable[(opa << 8) | val] = func(val, 0xFF - opa);
	}
	return &OpacityTable[opacity << 8];
}

static void BitmapRender(const sint32 width, const sint32 height, uint32* bitmap,
	const Color* pcolor, const Zoom* pzoom, chars meshtype, const sint32 meshcount, const Mesh* mesh)
{
	typedef union {uint32 rgba; struct {uint08 r, g, b, a;};} dib32;
	dib32* const bits = (dib32*) bitmap;
	const uint32 row = width;
	const sint32 clipl = 0;
	const sint32 clipt = 0;
	const sint32 clipr = width;
	const sint32 clipb = height;
	const uint32 src = 0xFF000000 |
		GetOpacityR(pcolor->r, pcolor->a) |
		GetOpacityG(pcolor->g, pcolor->a) |
		GetOpacityB(pcolor->b, pcolor->a);
	const uint32* rdst = GetDstOpacityTable<0>(pcolor->a, GetOpacityR);
	const uint32* gdst = GetDstOpacityTable<1>(pcolor->a, GetOpacityG);
	const uint32* bdst = GetDstOpacityTable<2>(pcolor->a, GetOpacityB);

	Zoom zoom = *pzoom; // 속도향상을 위해 지역으로 복사
	zoom.offset.x += width / 2.0f / zoom.scale;
	zoom.offset.y += -height / 2.0f / zoom.scale;

	auto proc_noclip = [=](const sint32 vl, const sint32 vr, const sint32 xl, const sint32 xr,
		const sint32 yb, const sint32 ye, const bool needxr) -> sint32
	{
		PX_ASSERT("파라미터오류", xl <= xr);
		const sint32 ybegin = yb;
		const sint32 yend = ye;
		sint32 xb = xl;
		sint32 xe = xr;

		dib32* ybits = &bits[row * ybegin];
		dib32* const ybitsend = &bits[row * yend];
		for(; ybits < ybitsend; ybits += row)
		{
			const sint32 xbegin = xb >> 16;
			const sint32 xend = xe >> 16;
			if(xbegin < xend)
			{
				dib32* dst = &ybits[xbegin];
				dib32* const dstend = &ybits[xend];
				switch(~(xend - xbegin - 1) & 0x3F)
				while(dst < dstend)
					PROCLOOP64((dst++)->rgba = src + (rdst[dst->r] | gdst[dst->g] | bdst[dst->b]);)
			}
			xb += vl;
			xe += vr;
		}
		return (needxr)? xe : xb;
	};

	auto proc_clip = [=](const sint32 vl, const sint32 vr, const sint32 xl, const sint32 xr,
		const sint32 yb, const sint32 ye, const bool needxr) -> sint32
	{
		PX_ASSERT("파라미터오류", xl <= xr);
		const sint32 ybegin = Math::Max(yb, clipt);
		const sint32 yend = Math::Min(ye, clipb);
		sint32 xb = xl + vl * (ybegin - yb);
		sint32 xe = xr + vr * (ybegin - yb);

		dib32* ybits = &bits[row * ybegin];
		dib32* const ybitsend = &bits[row * yend];
		for(; ybits < ybitsend; ybits += row)
		{
			const sint32 xbegin = Math::Max(xb >> 16, clipl);
			const sint32 xend = Math::Min(xe >> 16, clipr);
			if(xbegin < xend)
			{
				dib32* dst = &ybits[xbegin];
				dib32* const dstend = &ybits[xend];
				switch(~(xend - xbegin - 1) & 0x3F)
				while(dst < dstend)
					PROCLOOP64((dst++)->rgba = src + (rdst[dst->r] | gdst[dst->g] | bdst[dst->b]);)
			}
			xb += vl;
			xe += vr;
		}
		return (needxr)? xe : xb;
	};

	if(CONST_STRING("PlatyVGElement::MeshAqua") == meshtype)
	{
		const MeshAqua* Aquas = (const MeshAqua*) mesh;
		sint32 oldlx = 0;
		sint32 oldly = 0;
		sint32 oldrx = 0;
		sint32 oldry = 0;
		bool inoldl = false;
		bool inoldr = false;
		for(sint32 i = 0, iend = meshcount; i < iend; ++i)
		{
			const sint32 newlx = (sint32) ((Aquas[i].lx + zoom.offset.x) * zoom.scale);
			const sint32 newly = (sint32) ((Aquas[i].ly + zoom.offset.y) * -zoom.scale);
			const sint32 newrx = (sint32) ((Aquas[i].rx + zoom.offset.x) * zoom.scale);
			const sint32 newry = (sint32) ((Aquas[i].ry + zoom.offset.y) * -zoom.scale);
			const bool innewl = (clipl <= newlx && newlx < clipr && clipt <= newly && newly < clipb);
			const bool innewr = (clipl <= newrx && newrx < clipr && clipt <= newry && newry < clipb);
			if(0 < i)
			{
				if(inoldl && inoldr && innewl)
					PolyDraw(oldlx, oldly, oldrx, oldry, newlx, newly, proc_noclip);
				else if(inoldl || inoldr || innewl)
					PolyDraw(oldlx, oldly, oldrx, oldry, newlx, newly, proc_clip);
				if(innewr && innewl && inoldr)
					PolyDraw(newrx, newry, newlx, newly, oldrx, oldry, proc_noclip);
				else if(innewr || innewl || inoldr)
					PolyDraw(newrx, newry, newlx, newly, oldrx, oldry, proc_clip);
			}
			oldlx = newlx;
			oldly = newly;
			oldrx = newrx;
			oldry = newry;
			inoldl = innewl;
			inoldr = innewr;
		}
	}
}

class TempBuffer
{
	uint08* Buffer;
	sint32 Size;

public:
	TempBuffer() : Buffer(nullptr), Size(-1) {}
	~TempBuffer() {delete[] Buffer;}

	uint08* ValidBuffer(const sint32 size)
	{
		if(Size < size)
		{
			delete[] Buffer;
			Buffer = new uint08[Size = size];
		}
		return Buffer;
	}
};

namespace PlatyVGApi
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_long, Adapter_GetDocumentLink, api_int doc)
	PX_API_IMPL(return Adapter::GetDocumentLink(doc))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	linkstep1 Adapter::GetDocumentLink(const sint32 doc)
	{
		View* view = View::Currently();
		return (linkstep1) view->GetDocument(doc);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_int, Adapter_GetLayerCount, api_long doclink)
	PX_API_IMPL(return Adapter::GetLayerCount(doclink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Adapter::GetLayerCount(linkstep1 doclink)
	{
		PX_ASSERT("자료의 링크가 없습니다", doclink);
		const Document* document = (const Document*) doclink;
		return document->AllLayers.Count();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_long, Adapter_GetLayerLink, api_long doclink, api_int index)
	PX_API_IMPL(return Adapter::GetLayerLink(doclink, index))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	linkstep2 Adapter::GetLayerLink(linkstep1 doclink, const sint32 index)
	{
		PX_ASSERT("자료의 링크가 없습니다", doclink);
		const Document* document = (const Document*) doclink;
		return (linkstep2) &document->AllLayers[index];
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_int, Adapter_GetShapeCount, api_long layerlink, api_flag dyn)
	PX_API_IMPL(return Adapter::GetShapeCount(layerlink, dyn))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Adapter::GetShapeCount(linkstep2 layerlink, const bool dyn)
	{
		PX_ASSERT("레이어의 링크가 없습니다", layerlink);
		const Layer* layer = (const Layer*) layerlink;
		if(dyn) return layer->DynShapes.Count();
		return layer->AllShapes.Count();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_long, Adapter_GetShapeLink, api_long layerlink, api_flag dyn, api_int index)
	PX_API_IMPL(return Adapter::GetShapeLink(layerlink, dyn, index))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	linkstep3 Adapter::GetShapeLink(linkstep2 layerlink, const bool dyn, const sint32 index)
	{
		PX_ASSERT("레이어의 링크가 없습니다", layerlink);
		const Layer* layer = (const Layer*) layerlink;
		if(dyn) return (linkstep3) layer->DynShapes[index].ptr_const();
		return (linkstep3) layer->AllShapes[index].ptr_const();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_flag, Adapter_IsLayerVisibled, api_long layerlink)
	PX_API_IMPL(return Adapter::IsLayerVisibled(layerlink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const bool Adapter::IsLayerVisibled(linkstep2 layerlink)
	{
		PX_ASSERT("레이어의 링크가 없습니다", layerlink);
		const Layer* layer = (const Layer*) layerlink;
		return layer->Visible;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_flag, Adapter_IsShapeSelected, api_long shapelink)
	PX_API_IMPL(return Adapter::IsShapeSelected(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const bool Adapter::IsShapeSelected(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->IsSelected();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_int, Adapter_GetShapeColorRed, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeColorRed(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Adapter::GetShapeColorRed(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetContext().color.r & 0xFF;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_int, Adapter_GetShapeColorGreen, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeColorGreen(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Adapter::GetShapeColorGreen(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetContext().color.g & 0xFF;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_int, Adapter_GetShapeColorBlue, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeColorBlue(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Adapter::GetShapeColorBlue(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetContext().color.b & 0xFF;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_int, Adapter_GetShapeColorAlpha, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeColorAlpha(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Adapter::GetShapeColorAlpha(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetContext().color.a & 0xFF;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_int, Adapter_GetShapeColorAqua, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeColorAqua(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Adapter::GetShapeColorAqua(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetContext().color.aqua & 0xFF;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_float, Adapter_GetShapeMatrixM11, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeMatrixM11(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float Adapter::GetShapeMatrixM11(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetMatrix().m11;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_float, Adapter_GetShapeMatrixM12, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeMatrixM12(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float Adapter::GetShapeMatrixM12(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetMatrix().m12;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_float, Adapter_GetShapeMatrixM21, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeMatrixM21(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float Adapter::GetShapeMatrixM21(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetMatrix().m21;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_float, Adapter_GetShapeMatrixM22, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeMatrixM22(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float Adapter::GetShapeMatrixM22(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetMatrix().m22;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_float, Adapter_GetShapeMatrixDx, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeMatrixDx(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float Adapter::GetShapeMatrixDx(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetMatrix().dx;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_float, Adapter_GetShapeMatrixDy, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeMatrixDy(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float Adapter::GetShapeMatrixDy(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetMatrix().dy;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_string, Adapter_GetShapeMeshType, api_long shapelink)
	PX_API_IMPL_RETURN(Adapter::GetShapeMeshType(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	chars Adapter::GetShapeMeshType(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetMeshType();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_int, Adapter_GetShapeMeshCount, api_long shapelink)
	PX_API_IMPL(return Adapter::GetShapeMeshCount(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Adapter::GetShapeMeshCount(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		return shape->GetMeshCount();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_floats, Adapter_GetShapeMeshArray, api_long shapelink)
	PX_API_IMPL_RETURN(Adapter::GetShapeMeshArray(shapelink), Adapter::GetShapeMeshCount(shapelink))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float* Adapter::GetShapeMeshArray(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		if(shape->GetMeshCount() == 0) return nullptr;
		return (const float*) shape->GetMesh(0);
	}

	const sint32 Adapter::GetShapeBufferSize(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		const sint32 count = shape->GetEdgeCount();
		if(0 < count)
		{
			String name = shape->GetEdgeType();
			const uint08 namesize = (uint08) name.Length();
			const sint32 length = shape->GetEdge(0)->LengthOfWrite();
			return namesize + 1 + 12 + length * count;
		}
		return 0;
	}

	bytes Adapter::GetShapeBuffer(linkstep3 shapelink)
	{
		PX_ASSERT("도형의 링크가 없습니다", shapelink);
		const Shape* shape = (const Shape*) shapelink;
		const sint32 count = shape->GetEdgeCount();
		if(0 < count)
		{
			static TempBuffer temp;
			String name = shape->GetEdgeType();
			const uint08 namesize = (uint08) name.Length();
			const sint32 length = shape->GetEdge(0)->LengthOfWrite();
			// 버퍼확보
			uint08* shapebuffer = temp.ValidBuffer(namesize + 1 + 12 + length * count);
			uint08* buffer_name = shapebuffer;
			uint08* buffer_context = buffer_name + namesize + 1;
			uint08* buffer_edges = buffer_context + 12;
			// 헤더구성
			Memory::Copy(buffer_name, (chars) name, namesize + 1);
			const Context& context = shape->GetContext();
			Memory::Copy(buffer_context + 0, &context.color.rgba, 4);
			Memory::Copy(buffer_context + 4, &context.color.aqua, 4);
			Memory::Copy(buffer_context + 8, &context.thick, 4);
			// 내용구성
			for(sint32 i = 0; i < count; ++i)
				shape->GetEdge(i)->Write(buffer_edges + length * i);
			return shapebuffer;
		}
		return nullptr;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Adapter_RenderDIB, api_int doc, api_float x, api_float y, api_float scale, api_bytes bits, api_int width, api_int height)
    PX_API_IMPL(Adapter::RenderDIB(doc, Zoom(Point(x, y), scale), (uint32*)(bytes) PX_API_BRR(bits), width, height))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Adapter::RenderDIB(const sint32 doc, const Zoom zoom, uint32* bits, const sint32 width, const sint32 height)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(!document) return;

		for(sint32 l = 0, lend = document->AllLayers.Count(); l < lend; ++l)
		{
			const Layer* CurLayer = &document->AllLayers[l];
			if(!CurLayer->Visible) continue;
			for(sint32 i = 0, iend = CurLayer->AllShapes.Count(); i < iend; ++i)
			{
				const Shape* CurShape = CurLayer->AllShapes[i].ptr_const();
				const sint32 MeshCount = CurShape->GetMeshCount();
				if(MeshCount == 0) continue;
				BitmapRender(width, height, bits, &CurShape->GetContext().color,
					&zoom, CurShape->GetMeshType(), MeshCount, CurShape->GetMesh(0));
			}
			for(sint32 i = 0, iend = CurLayer->DynShapes.Count(); i < iend; ++i)
			{
				const Shape* CurShape = CurLayer->DynShapes[i].ptr_const();
				if(!CurShape) continue;
				const sint32 MeshCount = CurShape->GetMeshCount();
				if(MeshCount == 0) continue;
				BitmapRender(width, height, bits, &CurShape->GetContext().color,
					&zoom, CurShape->GetMeshType(), MeshCount, CurShape->GetMesh(0));
			}
		}
	}
}
