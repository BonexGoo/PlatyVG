#include <platyvg.hpp>
#include "command.hpp"
#include "input.hpp"
#include "../core/document.hpp"
#include "../core/view.hpp"
#include "../element/selector.hpp"

namespace PlatyVGApi
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Input_ScreenDown, api_uint id, api_float x, api_float y, api_float force, api_enum deviceby)
	PX_API_IMPL(Input::ScreenDown(id, x, y, force, (DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::ScreenDown(uint32 id, float x, float y, float force, const DeviceBy deviceby)
	{
		View* view = View::Currently();
		Device* device = &view->DeviceState[deviceby];
		device->InputFlag |= 1 << id;
		if(id < 2) device->Gesture[id] = Point(x, y);

		Document* doc = view->DocumentFocus;
		if(doc)
		{
			if((device->InputFlag & 0x3) == 0x3)
			{
				device->IsZoomEnabled = true;
				device->IsOutsideDown = false;
				device->InputCoords.Clear();
				view->UnlockDynShape(doc, deviceby, false);
			}
			else if(!device->IsZoomEnabled && !device->IsOutsideDown)
			{
				if((doc->Area * doc->Transform).PtInRect(x, y))
				{
					device->InputCoords.Clear();
					view->UnlockDynShape(doc, deviceby, false); // Case of lost up-event
					view->LockDynShape(doc, deviceby, view->OptionStrokeBy);
					ScreenDrag(id, x, y, force, deviceby);
				}
				// 뷰포커싱
				else device->IsOutsideDown = true;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Input_ScreenDrag, api_uint id, api_float x, api_float y, api_float force, api_enum deviceby)
	PX_API_IMPL(Input::ScreenDrag(id, x, y, force, (DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::ScreenDrag(uint32 id, float x, float y, float force, const DeviceBy deviceby)
	{
		View* view = View::Currently();
		Device* device = &view->DeviceState[deviceby];
		Point NewGesture[2] = {device->Gesture[0], device->Gesture[1]};
		if(id < 2) NewGesture[id] = Point(x, y);

		Document* doc = view->DocumentFocus;
		if(doc)
		{
			if((device->InputFlag & 0x3) == 0x3)
			{
				bool IsAnybodySelected = false;
				if(view->OptionStrokeBy == StrokeBySelect)
				{
					for(sint32 l = 0, lend = doc->AllLayers.Count(); l < lend; ++l)
					for(sint32 i = 0, iend = doc->AllLayers[l].AllShapes.Count(); i < iend; ++i)
					{
						Shape* CurShape = doc->AllLayers.At(l).AllShapes.At(i).ptr();
						if(CurShape->IsSelected())
						{
							IsAnybodySelected = true;
							CurShape->ChangeMatrix(device->Gesture, NewGesture, doc->Transform);
						}
					}
				}
				if(!IsAnybodySelected)
					doc->Transform.UpdateBy(device->Gesture, NewGesture);
			}
			else if(!device->IsZoomEnabled && !device->IsOutsideDown)
			{
                view->AddCoordSensitivity(deviceby,
                    x / doc->Transform.scale - doc->Transform.offset.x,
                    y / doc->Transform.scale - doc->Transform.offset.y,
                    force * device->GetSpeedyForce(x, y, view->OptionMS));
				Shape* CurDynShape = view->GetDynShape(doc, deviceby);
				if(CurDynShape)
				{
					Context ScreenContext = Context(view->OptionContext.color,
						view->OptionContext.thick / doc->Transform.scale);
					CurDynShape->Process(ScreenContext, device->InputCoords);
					CurDynShape->Update(doc->Transform.offset, doc->Transform.scale, view->OptionRadius);
				}
				else if(view->OptionStrokeBy == StrokeByErase && 1 < device->InputCoords.Count())
				{
					Coords EraseCoords;
					EraseCoords.AtAdding() = device->InputCoords[-2];
					EraseCoords.AtAdding() = device->InputCoords[-1];
					Selector EraseSelector = Selector(EraseCoords);
                    for(sint32 l = doc->AllLayers.Count() - 1; 0 <= l; --l)
					{
						if(!doc->AllLayers[l].Visible) continue;
                        for(sint32 i = doc->AllLayers[l].AllShapes.Count() - 1; 0 <= i; --i)
						{
							Shape* CurShape = doc->AllLayers.At(l).AllShapes.At(i).ptr();
							if(CurShape->SelectTest(SelectByDrag, EraseSelector))
                            {
								CurShape->Reset();
								doc->LastMS = view->OptionMS;
                                l = i = 0;
                            }
						}
					}
				}
				else if(view->OptionStrokeBy == StrokeByHand)
					doc->Transform.UpdateBy(device->Gesture[id], NewGesture[id], 1);
				else if(view->OptionStrokeBy == StrokeBySpoid)
				{
					const Color SpoidColor = Command::GetColorBy(Point(
						x / doc->Transform.scale - doc->Transform.offset.x,
						y / doc->Transform.scale - doc->Transform.offset.y),
						Color(255, 255, 255, 255, 0));
					view->OptionContext.color.r = SpoidColor.r;
					view->OptionContext.color.g = SpoidColor.g;
					view->OptionContext.color.b = SpoidColor.b;
				}
			}
		}
		else if((device->InputFlag & 0x3) == 0x3)
		{
			device->IsGlobalZoomEnabled = true;
			doc = &view->DocumentRoot;
			while(doc = doc->Next)
				doc->Transform.UpdateBy(device->Gesture, NewGesture);
		}
		else if(view->OptionStrokeBy == StrokeByHand)
		{
			doc = &view->DocumentRoot;
			while(doc = doc->Next)
				doc->Transform.UpdateBy(device->Gesture[id], NewGesture[id], 1);
		}

		device->Gesture[id] = NewGesture[id];
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Input_ScreenUp, api_uint id, api_enum deviceby)
	PX_API_IMPL(Input::ScreenUp(id, (DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::ScreenUp(uint32 id, const DeviceBy deviceby)
	{
		View* view = View::Currently();
		Device* device = &view->DeviceState[deviceby];
		device->InputFlag &= ~(1 << id);

		if((device->InputFlag & 0x3) == 0x0)
		{
			Document* doc = view->DocumentFocus;
			if(doc)
			{
				if(device->IsZoomEnabled)
				{
					device->IsZoomEnabled = false;
					doc->UpdateLayers(view->OptionRadius);
				}
				else if(view->OptionStrokeBy == StrokeBySelect)
				{
					Selector LastSelector = Selector(device->InputCoords);
					for(sint32 l = 0, lend = doc->AllLayers.Count(); l < lend; ++l)
					{
						if(!doc->AllLayers[l].Visible) continue;
						for(sint32 i = 0, iend = doc->AllLayers[l].AllShapes.Count(); i < iend; ++i)
						{
							Shape* CurShape = doc->AllLayers.At(l).AllShapes.At(i).ptr();
							CurShape->Select(view->OptionSelectBy, LastSelector, MergeByPlus);
						}
					}
				}
				else if(view->OptionStrokeBy == StrokeByHand)
					doc->UpdateLayers(view->OptionRadius);
				// 뷰포커싱
				if(device->IsOutsideDown)
				{
					device->IsOutsideDown = false;
                    sint32 DocID = -1;
                    doc = &view->DocumentRoot;
					while(doc = doc->Next)
						if(doc->Focusable && (doc->Area * doc->Transform).PtInRect(device->Gesture[0]))
                            DocID = doc->ID;
                    Command::Canvas::SetFocus(DocID);
				}
				device->InputCoords.Clear();
				if(view->UnlockDynShape(doc, deviceby, true))
					doc->LastMS = view->OptionMS;
			}
			else
			{
				if(device->IsGlobalZoomEnabled)
				{
					device->IsGlobalZoomEnabled = false;
					doc = &view->DocumentRoot;
					while(doc = doc->Next)
						doc->UpdateLayers(view->OptionRadius);
				}
				else
				{
                    sint32 DocID = -1;
					doc = &view->DocumentRoot;
					while(doc = doc->Next)
						if(doc->Focusable && (doc->Area * doc->Transform).PtInRect(device->Gesture[0]))
                            DocID = doc->ID;
                    Command::Canvas::SetFocus(DocID);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Input_CanvasDown, api_float x, api_float y, api_float force, api_enum deviceby)
	PX_API_IMPL(Input::CanvasDown(x, y, force, (DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::CanvasDown(float x, float y, float force, const DeviceBy deviceby)
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		if(doc)
		{
			view->DeviceState[deviceby].InputCoords.Clear();
			view->UnlockDynShape(doc, deviceby, false); // Case of lost up-event
			view->LockDynShape(doc, deviceby, StrokeByAqua);
			CanvasDrag(x, y, force, deviceby);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Input_CanvasDrag, api_float x, api_float y, api_float force, api_enum deviceby)
	PX_API_IMPL(Input::CanvasDrag(x, y, force, (DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::CanvasDrag(float x, float y, float force, const DeviceBy deviceby)
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		if(doc)
		{
			Shape* CurDynShape = view->GetDynShape(doc, deviceby);
			if(CurDynShape)
			{
				Device* device = &view->DeviceState[deviceby];
				view->AddCoordSensitivity(deviceby, x, y,
					force * device->GetSpeedyForce(x, y, view->OptionMS));
				CurDynShape->Process(view->OptionContext, device->InputCoords);
				CurDynShape->Update(doc->Transform.offset, doc->Transform.scale, view->OptionRadius);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Input_CanvasUp, api_enum deviceby)
	PX_API_IMPL(Input::CanvasUp((DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::CanvasUp(const DeviceBy deviceby)
	{
		View* view = View::Currently();
		Document* doc = view->DocumentFocus;
		if(doc)
		{
			view->DeviceState[deviceby].InputCoords.Clear();
			if(view->UnlockDynShape(doc, deviceby, true))
				doc->LastMS = view->OptionMS;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Input_SetDeviceSensitivity, api_enum deviceby, api_float sensitivity)
	PX_API_IMPL(Input::SetDeviceSensitivity((DeviceBy) deviceby, sensitivity))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::SetDeviceSensitivity(const DeviceBy deviceby, float sensitivity)
	{
		View* view = View::Currently();
		view->DeviceState[deviceby].InputSensitivity = sensitivity;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_float, Input_GetDeviceSensitivity, api_enum deviceby)
	PX_API_IMPL(return Input::GetDeviceSensitivity((DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float Input::GetDeviceSensitivity(const DeviceBy deviceby)
	{
		View* view = View::Currently();
		return view->DeviceState[deviceby].InputSensitivity;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(void, Input_SetDeviceSpeedyForceOn, api_enum deviceby, api_float distancebysecond)
	PX_API_IMPL(Input::SetDeviceSpeedyForceOn((DeviceBy) deviceby, distancebysecond))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::SetDeviceSpeedyForceOn(const DeviceBy deviceby, float distancebysecond)
	{
		View* view = View::Currently();
		view->DeviceState[deviceby].InputBaseDistanceByMS = distancebysecond / 1000;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(void, Input_SetDeviceSpeedyForceOff, api_enum deviceby)
	PX_API_IMPL(Input::SetDeviceSpeedyForceOff((DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::SetDeviceSpeedyForceOff(const DeviceBy deviceby)
	{
		View* view = View::Currently();
		view->DeviceState[deviceby].InputBaseDistanceByMS = 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(void, Input_SetDeviceLayer, api_enum deviceby, api_int layer)
	PX_API_IMPL(Input::SetDeviceLayer((DeviceBy) deviceby, layer))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Input::SetDeviceLayer(const DeviceBy deviceby, const sint32 layer)
	{
		View* view = View::Currently();
		view->DeviceLayer[deviceby] = layer;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_int, Input_GetDeviceLayer, api_enum deviceby)
	PX_API_IMPL(return Input::GetDeviceLayer((DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Input::GetDeviceLayer(const DeviceBy deviceby)
	{
		View* view = View::Currently();
		return view->DeviceLayer[deviceby];
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PX_API(api_int, Input_GetDeviceCoordCount, api_enum deviceby)
	PX_API_IMPL(return Input::GetDeviceCoordCount((DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const sint32 Input::GetDeviceCoordCount(const DeviceBy deviceby)
	{
		View* view = View::Currently();
		return view->DeviceState[deviceby].InputCoords.Count();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_floats, Input_GetDeviceCoordArray, api_enum deviceby)
    PX_API_IMPL_RETURN(Input::GetDeviceCoordArray((DeviceBy) deviceby), Input::GetDeviceCoordCount((DeviceBy) deviceby))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float* Input::GetDeviceCoordArray(const DeviceBy deviceby)
	{
		View* view = View::Currently();
		return &view->DeviceState[deviceby].InputCoords[0].x;
	}
}
