#pragma once
#include <platyvg.hpp>
#include "../element/coord.hpp"
#include "../element/point.hpp"

namespace PlatyVGCore
{
	//! \brief 장치관리
	class Device
	{
	public:
		Device();
		~Device();

	public:
		bool IsZoomEnabled;
		bool IsOutsideDown;
		bool IsGlobalZoomEnabled;
		uint32 InputFlag;
		Point Gesture[2];
		Coords InputCoords;
		sint32 InputLocked;
		float InputSensitivity;
		float InputBaseDistanceByMS;

	protected:
		float CurrentSpeedyForce;
		float OldXForSpeedyForce;
		float OldYForSpeedyForce;
		sint64 OldMSForSpeedyForce;

	public:
		float GetSpeedyForce(float x, float y, sint64 ms);
	};
}
