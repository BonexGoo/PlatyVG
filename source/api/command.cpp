#include <platyvg.hpp>
#include "command.hpp"
#include "../core/document.hpp"
#include "../core/record.hpp"
#include "../core/view.hpp"

namespace PlatyVGApi
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_int, Command_Canvas_Create, api_flag focusable)
    PX_API_IMPL(return Command::Canvas::Create(focusable))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Command::Canvas::Create(const bool focusable)
	{
		View* view = View::Currently();
		Document* CurDocument = &view->DocumentRoot;
		// 후배치
		while(CurDocument->Next) CurDocument = CurDocument->Next;
		CurDocument->Next = new Document(focusable);
		return CurDocument->Next->ID;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_Canvas_Remove, api_int doc)
	PX_API_IMPL(Command::Canvas::Remove(doc))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::Canvas::Remove(const sint32 doc)
	{
		View* view = View::Currently();
		Document* CurDocument = &view->DocumentRoot;
		while(CurDocument->Next)
		{
			if(CurDocument->Next->ID == doc)
			{
				// 분리
				Document* OneDocument = CurDocument->Next;
				CurDocument->Next = OneDocument->Next;
				OneDocument->Next = nullptr;
				// 포커스처리
				if(view->DocumentFocus == OneDocument)
					view->DocumentFocus = nullptr;
				// 제거
				delete OneDocument;
				break;
			}
			CurDocument = CurDocument->Next;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_Canvas_Update, api_int doc)
	PX_API_IMPL(Command::Canvas::Update(doc))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::Canvas::Update(const sint32 doc)
	{
		View* view = View::Currently();
		UpdateBy(doc, view->OptionRadius);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_Canvas_UpdateBy, api_int doc, api_float radius)
	PX_API_IMPL(Command::Canvas::UpdateBy(doc, radius))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::Canvas::UpdateBy(const sint32 doc, const float radius)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(document) document->UpdateLayers(radius);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_Canvas_SetName, api_int doc, api_string name)
    PX_API_IMPL(Command::Canvas::SetName(doc, PX_API_STR(name)))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Command::Canvas::SetName(const sint32 doc, chars name)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(document) document->Name = name;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_string, Command_Canvas_GetName, api_int doc)
    PX_API_IMPL_RETURN(Command::Canvas::GetName(doc))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const String Command::Canvas::GetName(const sint32 doc)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(document) return document->Name;
		return String();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_Canvas_SetArea, api_int doc, api_float l, api_float t, api_float r, api_float b)
	PX_API_IMPL(Command::Canvas::SetArea(doc, Rect(l, t, r, b)))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::Canvas::SetArea(const sint32 doc, const Rect rect)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(document) document->Area = rect;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_results, Command_Canvas_GetArea, api_int doc)
    PX_API_IMPL_RESULTS(const Rect rect = Command::Canvas::GetArea(doc), rect.l, rect.t, rect.r, rect.b)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const Rect Command::Canvas::GetArea(const sint32 doc)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(document) return document->Area;
		return Rect();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_Canvas_SetZoom, api_int doc, api_float x, api_float y, api_float scale)
	PX_API_IMPL(Command::Canvas::SetZoom(doc, Zoom(Point(x, y), scale)))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::Canvas::SetZoom(const sint32 doc, const Zoom zoom)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(document) document->Transform = zoom;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_Canvas_SetZoomInRect, api_int doc, api_float l, api_float t, api_float r, api_float b, api_enum boundby)
	PX_API_IMPL(Command::Canvas::SetZoomInRect(doc, Rect(l, t, r, b), (BoundBy) boundby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::Canvas::SetZoomInRect(const sint32 doc, const Rect rect, const BoundBy boundby)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(document)
		{
			const float WidthRate = rect.Width() / document->Area.Width();
			const float HeightRate = rect.Height() / document->Area.Height();
			float Rate = 0;
			switch(boundby)
			{
			case BoundByWidth: Rate = WidthRate; break;
			case BoundByHeight: Rate = HeightRate; break;
			case BoundByInner: Rate = (WidthRate < HeightRate)? WidthRate : HeightRate; break;
			case BoundByOuter: Rate = (WidthRate > HeightRate)? WidthRate : HeightRate; break;
			}
			document->Transform = Zoom(Point(rect.CenterX() / Rate, rect.CenterY() / Rate), Rate);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_results, Command_Canvas_GetZoom, api_int doc)
    PX_API_IMPL_RESULTS(const Zoom z = Command::Canvas::GetZoom(doc), z.offset.x, z.offset.y, z.scale)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const Zoom Command::Canvas::GetZoom(const sint32 doc)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(document) return document->Transform;
		return Zoom();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_Canvas_SetFocus, api_int doc, api_flag popup)
	PX_API_IMPL(Command::Canvas::SetFocus(doc, popup))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Command::Canvas::SetFocus(const sint32 doc, const bool popup)
	{
		View* view = View::Currently();
        view->DocumentFocus = nullptr;
		if(doc != -1)
		{
			Document* CurDocument = &view->DocumentRoot;
			while(CurDocument->Next)
			{
				if(CurDocument->Next->ID == doc)
				{
					if(CurDocument->Next->Focusable)
					{
						view->DocumentFocus = CurDocument->Next;
						if(popup)
						{
							// 분리
							Document* OneDocument = CurDocument->Next;
							CurDocument->Next = OneDocument->Next;
							OneDocument->Next = nullptr;
							// 후배치
							while(CurDocument->Next) CurDocument = CurDocument->Next;
							CurDocument->Next = OneDocument;
						}
					}
					return;
				}
				CurDocument = CurDocument->Next;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(api_int, Command_Canvas_GetFocus)
	PX_API_IMPL(return Command::Canvas::GetFocus())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Command::Canvas::GetFocus()
	{
		View* view = View::Currently();
		if(!view->DocumentFocus) return -1;
		return view->DocumentFocus->ID;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(api_int, Command_Canvas_GetDocumentCount)
	PX_API_IMPL(return Command::Canvas::GetDocumentCount())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Command::Canvas::GetDocumentCount()
	{
		sint32 Result = 0;
		View* view = View::Currently();
		Document* CurDocument = &view->DocumentRoot;
		while(CurDocument = CurDocument->Next) ++Result;
		return Result;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_int, Command_Canvas_GetDocument, api_int index)
	PX_API_IMPL(return Command::Canvas::GetDocument(index))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Command::Canvas::GetDocument(const sint32 index)
	{
		View* view = View::Currently();
		Document* CurDocument = &view->DocumentRoot;
		for(sint32 i = 0; CurDocument = CurDocument->Next; ++i)
			if(i == index) return CurDocument->ID;
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_int, Command_Canvas_GetDocumentBy, api_float x, api_float y)
	PX_API_IMPL(return Command::Canvas::GetDocumentBy(Point(x, y)))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Command::Canvas::GetDocumentBy(const Point point)
	{
		View* view = View::Currently();
		Document* CurDocument = &view->DocumentRoot;
		while(CurDocument = CurDocument->Next)
		{
			const Rect CurRect = CurDocument->Area * CurDocument->Transform;
			if(CurRect.PtInRect(point))
				return CurDocument->ID;
		}
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_long, Command_Canvas_GetTimeOfLastDrawing, api_int doc)
	PX_API_IMPL(return Command::Canvas::GetTimeOfLastDrawing(doc))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint64 Command::Canvas::GetTimeOfLastDrawing(const sint32 doc)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(document) return document->LastMS;
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_Canvas_Snapshot, api_int doc, api_bytes bits, api_int width, api_int height, api_enum boundby)
    PX_API_IMPL(Command::Canvas::Snapshot(doc, (uint32*)(bytes) PX_API_BRR(bits), width, height, (BoundBy) boundby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::Canvas::Snapshot(const sint32 doc, uint32* bits, const sint32 width, const sint32 height, const BoundBy boundby)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(!document) return;

		const float WidthRate = width / document->Area.Width();
		const float HeightRate = height / document->Area.Height();
		float Rate = 0;
		switch(boundby)
		{
		case BoundByWidth: Rate = WidthRate; break;
		case BoundByHeight: Rate = HeightRate; break;
		case BoundByInner: Rate = (WidthRate < HeightRate)? WidthRate : HeightRate; break;
		case BoundByOuter: Rate = (WidthRate > HeightRate)? WidthRate : HeightRate; break;
		}

		const Zoom OldTransform = document->Transform;
		const Zoom NewTransform = Zoom(Point(), Rate);
		const bool NeedUpdate = (OldTransform != NewTransform);
		if(NeedUpdate)
		{
			document->Transform = NewTransform;
			document->UpdateLayers(Math::Sqrt((width * width + height * height) / 4));
		}

		Adapter::RenderDIB(doc, document->Transform, bits, width, height);

		if(NeedUpdate)
		{
			document->Transform = OldTransform;
			document->UpdateLayers(view->OptionRadius);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_int, Command_GetShapeCount, api_int doc, api_int layer)
	PX_API_IMPL(return Command::GetShapeCount(doc, layer))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	sint32 Command::GetShapeCount(const sint32 doc, const sint32 layer)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(!document) return 0;

		sint32 ShapeCount = 0;
		if(layer == -1)
		{
			for(sint32 l = 0, lend = document->AllLayers.Count(); l < lend; ++l)
				ShapeCount += document->AllLayers.At(l).AllShapes.Count();
		}
		else if(0 <= layer && layer < document->AllLayers.Count())
			ShapeCount = document->AllLayers.At(layer).AllShapes.Count();
		return ShapeCount;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_ClearShapes, api_int doc, api_int layer)
	PX_API_IMPL(Command::ClearShapes(doc, layer))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::ClearShapes(const sint32 doc, const sint32 layer)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		if(!document) return;

		if(layer == -1)
		{
			for(sint32 l = 0, lend = document->AllLayers.Count(); l < lend; ++l)
				document->AllLayers.At(l).AllShapes.Clear();
		}
		else if(0 <= layer && layer < document->AllLayers.Count())
			document->AllLayers.At(layer).AllShapes.Clear();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(void, Command_ClearSelects)
	PX_API_IMPL(Command::ClearSelects())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::ClearSelects()
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		PX_ASSERT("포커싱된 자료가 없습니다", doc);
		if(!doc) return;

		for(sint32 l = 0, lend = doc->AllLayers.Count(); l < lend; ++l)
		for(sint32 i = 0, iend = doc->AllLayers[l].AllShapes.Count(); i < iend; ++i)
			doc->AllLayers.At(l).AllShapes.At(i)->Unselect();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(void, Command_RemoveSelects)
	PX_API_IMPL(Command::RemoveSelects())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::RemoveSelects()
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		PX_ASSERT("포커싱된 자료가 없습니다", doc);
		if(!doc) return;

		for(sint32 l = 0, lend = doc->AllLayers.Count(); l < lend; ++l)
		for(sint32 i = 0, iend = doc->AllLayers[l].AllShapes.Count(); i < iend; ++i)
		{
			Shape* CurShape = doc->AllLayers.At(l).AllShapes.At(i).ptr();
			if(CurShape->IsSelected()) CurShape->Reset();
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_ChangeSelects, api_uint rgba, api_uint aqua, api_enum changecolorby)
	PX_API_IMPL(Command::ChangeSelects(Color(rgba, aqua), (ChangeColorBy) changecolorby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::ChangeSelects(const Color color, const ChangeColorBy changecolorby)
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		PX_ASSERT("포커싱된 자료가 없습니다", doc);
		if(!doc) return;

		const bool DoRGB = (changecolorby == ChangeColorByRGB || changecolorby == ChangeColorByRGBAlpha ||
			changecolorby == ChangeColorByRGBAqua || changecolorby == ChangeColorByRGBAlphaAqua);
		const bool DoAlpha = (changecolorby == ChangeColorByAlpha || changecolorby == ChangeColorByRGBAlpha ||
			changecolorby == ChangeColorByAlphaAqua || changecolorby == ChangeColorByRGBAlphaAqua);
		const bool DoAqua = (changecolorby == ChangeColorByAqua || changecolorby == ChangeColorByRGBAqua ||
			changecolorby == ChangeColorByAlphaAqua || changecolorby == ChangeColorByRGBAlphaAqua);
		for(sint32 l = 0, lend = doc->AllLayers.Count(); l < lend; ++l)
		for(sint32 i = 0, iend = doc->AllLayers[l].AllShapes.Count(); i < iend; ++i)
		{
			Shape* CurShape = doc->AllLayers.At(l).AllShapes.At(i).ptr();
			if(CurShape->IsSelected())
			{
				if(DoRGB) CurShape->ChangeRGB(color.r, color.g, color.b);
				if(DoAlpha) CurShape->ChangeAlpha(color.a);
				if(DoAqua) CurShape->ChangeAqua(color.aqua);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_SetStrokeBy, api_string name)
    PX_API_IMPL(Command::SetStrokeBy(PX_API_STR(name)))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::SetStrokeBy(chars name)
	{
		View* view = View::Currently();
		if(CONST_STRING("spline") == name) view->OptionStrokeBy = StrokeBySpline;
		else if(CONST_STRING("tube") == name) view->OptionStrokeBy = StrokeByTube;
		else if(CONST_STRING("aqua") == name) view->OptionStrokeBy = StrokeByAqua;
		else if(CONST_STRING("erase") == name) view->OptionStrokeBy = StrokeByErase;
		else if(CONST_STRING("select") == name) view->OptionStrokeBy = StrokeBySelect;
		else if(CONST_STRING("hand") == name) view->OptionStrokeBy = StrokeByHand;
		else if(CONST_STRING("spoid") == name) view->OptionStrokeBy = StrokeBySpoid;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_SetSelectBy, api_string name)
    PX_API_IMPL(Command::SetSelectBy(PX_API_STR(name)))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::SetSelectBy(chars name)
	{
		View* view = View::Currently();
		if(CONST_STRING("rect") == name) view->OptionSelectBy = SelectByRect;
		else if(CONST_STRING("drag") == name) view->OptionSelectBy = SelectByDrag;
		else if(CONST_STRING("area") == name) view->OptionSelectBy = SelectByArea;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_SetColor, api_uint rgba, api_uint aqua)
	PX_API_IMPL(Command::SetColor(Color(rgba, aqua)))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::SetColor(const Color color)
	{
		View* view = View::Currently();
		view->OptionContext.color = color;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(api_results, Command_GetColor)
    PX_API_IMPL_RESULTS(const Color color = Command::GetColor(), color.rgba, color.aqua)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const Color Command::GetColor()
	{
		View* view = View::Currently();
		return view->OptionContext.color;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_results, Command_GetColorBy, api_float x, api_float y, api_uint bg_rgba, api_uint bg_aqua)
    PX_API_IMPL_RESULTS(const Color color = Command::GetColorBy(Point(x, y), Color(bg_rgba, bg_aqua)), color.rgba, color.aqua)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const Color Command::GetColorBy(const Point point, const Color backcolor)
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		PX_ASSERT("포커싱된 자료가 없습니다", doc);
		if(!doc) return backcolor;

		Coords SpoidCoords;
		SpoidCoords.AtAdding() = Coord(point.x - 10, point.y - 10, 0);
		SpoidCoords.AtAdding() = Coord(point.x + 10, point.y + 10, 0);
		Selector SpoidSelector = Selector(SpoidCoords);
		for(sint32 l = 0, lend = doc->AllLayers.Count(); l < lend; ++l)
		{
			if(!doc->AllLayers[l].Visible) continue;
			for(sint32 i = 0, iend = doc->AllLayers[l].AllShapes.Count(); i < iend; ++i)
			{
				Shape* CurShape = doc->AllLayers.At(l).AllShapes.At(i).ptr();
				if(CurShape->SelectTest(SelectByRect, SpoidSelector))
					return CurShape->GetContext().color;
			}
		}
		return backcolor;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_SetThick, api_float thick)
	PX_API_IMPL(Command::SetThick(thick))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::SetThick(const float thick)
	{
		View* view = View::Currently();
		view->OptionContext.thick = thick;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(api_float, Command_GetThick)
	PX_API_IMPL(return Command::GetThick())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float Command::GetThick()
	{
		View* view = View::Currently();
		return view->OptionContext.thick;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_SetViewRadius, api_float radius, api_flag updateAllDocuments)
	PX_API_IMPL(Command::SetViewRadius(radius, updateAllDocuments))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Command::SetViewRadius(const float radius, const bool updateAllDocuments)
	{
		View* view = View::Currently();
		view->OptionRadius = radius;

		if(updateAllDocuments)
		{
			Document* doc = &view->DocumentRoot;
			while(doc = doc->Next)
				doc->UpdateLayers(view->OptionRadius);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(api_float, Command_GetViewRadius)
	PX_API_IMPL(return Command::GetViewRadius())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float Command::GetViewRadius()
	{
		View* view = View::Currently();
		return view->OptionRadius;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_SetLayerVisible, api_int layer, api_flag visible)
	PX_API_IMPL(Command::SetLayerVisible(layer, visible))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Command::SetLayerVisible(const sint32 layer, const bool visible)
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		PX_ASSERT("포커싱된 자료가 없습니다", doc);
		if(!doc) return;

		doc->AllLayers.AtWherever(layer).Visible = visible;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_flag, Command_GetLayerVisible, api_int layer)
	PX_API_IMPL(return Command::GetLayerVisible(layer))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const bool Command::GetLayerVisible(const sint32 layer)
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		PX_ASSERT("포커싱된 자료가 없습니다", doc);
        if(!doc) return false;

		if(layer < 0 || doc->AllLayers.Count() <= layer)
			return false;
		return doc->AllLayers[layer].Visible;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Command_SetCurrentTime, api_long ms)
	PX_API_IMPL(Command::SetCurrentTime(ms))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Command::SetCurrentTime(const sint64 ms)
	{
		View* view = View::Currently();
		view->OptionMS = ms;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(api_flag, Command_Undo)
	PX_API_IMPL(return Command::Undo())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Command::Undo()
	{
		if(!CanUndo()) return false;
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		Record::Undo(doc);
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(api_flag, Command_Redo)
	PX_API_IMPL(return Command::Redo())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Command::Redo()
	{
		if(!CanRedo()) return false;
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		Record::Redo(doc);
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(api_flag, Command_CanUndo)
	PX_API_IMPL(return Command::CanUndo())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Command::CanUndo()
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		if(!doc) return false;
		return (0 < Record::GetFocus(doc));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API_VOID(api_flag, Command_CanRedo)
	PX_API_IMPL(return Command::CanRedo())
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Command::CanRedo()
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		if(!doc) return false;
		return (Record::GetFocus(doc) < Record::GetCount(doc));
	}
}
