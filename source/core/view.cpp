#include <platyvg.hpp>
#include "view.hpp"

namespace PlatyVGCore
{
	View::View() : DocumentRoot(false)
	{
		DocumentFocus = nullptr;
		OptionStrokeBy = StrokeBySpline;
		OptionSelectBy = SelectByRect;
		OptionRadius = 300;
		OptionMS = 0;

		PX_ASSERT("LengthOfDevice가 변경되었습니다", LengthOfDevice == 3);
		DeviceState[DeviceByTouch].InputSensitivity = 0.05f;
		DeviceState[DeviceByTablet].InputSensitivity = 0.95f;
		DeviceState[DeviceByPen].InputSensitivity = 0.8f;
		DeviceLayer[DeviceByTouch] = 0;
		DeviceLayer[DeviceByTablet] = 0;
		DeviceLayer[DeviceByPen] = 0;
	}

	View::~View()
	{
	}

	View* View::Currently()
	{
		static View CurrentlyView;
		return &CurrentlyView;
	}

	Document* View::GetDocument(sint32 doc)
	{
		Document* CurDocument = &DocumentRoot;
		while(CurDocument = CurDocument->Next)
		{
			if(CurDocument->ID != doc) continue;
			return CurDocument;
		}
		return nullptr;
	}

	void View::LockDynShape(Document* doc, const DeviceBy deviceby, const StrokeBy strokeby)
	{
		Device* device = &DeviceState[deviceby];
		if(device->InputLocked != -1) return;
		Layer& CurLayer = doc->AllLayers.AtWherever(DeviceLayer[deviceby]);

		for(sint32 i = 0; i < 32; ++i)
		{
			if(CurLayer.LockedFlag & (1 << i)) continue;
			UnionShape& CurShape = CurLayer.DynShapes.AtWherever(i);
			switch(strokeby)
			{
			case StrokeBySpline: CurShape = Buffer::Alloc<ShapeSpline>(NEEDS 1); break;
			case StrokeByTube: CurShape = Buffer::Alloc<ShapeTube>(NEEDS 1); break;
			case StrokeByAqua: CurShape = Buffer::Alloc<ShapeTubeAqua>(NEEDS 1); break;
			default: return;
			}
			CurLayer.LockedFlag |= 1 << i;
			device->InputLocked = i;
			return;
		}
		PX_ASSERT("동적도형을 확보하지 못했습니다", false);
	}

	bool View::UnlockDynShape(Document* doc, const DeviceBy deviceby, const bool addition)
	{
		Device* device = &DeviceState[deviceby];
		if(device->InputLocked == -1) return false;
		Layer& CurLayer = doc->AllLayers.At(DeviceLayer[deviceby]);
		UnionShape& CurShape = CurLayer.DynShapes.At(device->InputLocked);

		bool Result = false;
		CurLayer.LockedFlag &= ~(1 << device->InputLocked);
		device->InputLocked = -1;
		if(addition && (Result = (0 < CurShape->GetEdgeCount())))
			CurLayer.AllShapes.AtAdding() = CurShape.copied_buffer();
		CurShape->Reset();
		return Result;
	}

	Shape* View::GetDynShape(Document* doc, const DeviceBy deviceby)
	{
		Device* device = &DeviceState[deviceby];
		if(device->InputLocked == -1) return nullptr;
		Layer& CurLayer = doc->AllLayers.At(DeviceLayer[deviceby]);
		UnionShape& CurShape = CurLayer.DynShapes.At(device->InputLocked);
		return CurShape.ptr();
	}

	void View::AddCoordSensitivity(const DeviceBy deviceby, float x, float y, float force)
	{
		Device* device = &DeviceState[deviceby];
		Coords& CurCoords = device->InputCoords;
		if(1 < CurCoords.Count())
		{
			const float MiddleX = (x + CurCoords[-2].x) / 2;
			const float MiddleY = (y + CurCoords[-2].y) / 2;
			const float RevSensitivity = 1 - device->InputSensitivity;
			CurCoords.At(-1).x = CurCoords[-1].x * device->InputSensitivity + MiddleX * RevSensitivity;
			CurCoords.At(-1).y = CurCoords[-1].y * device->InputSensitivity + MiddleY * RevSensitivity;
		}
		CurCoords.AtAdding() = Coord(x, y, force);
	}
}
