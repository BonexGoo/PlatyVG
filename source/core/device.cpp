#include <platyvg.hpp>
#include "device.hpp"

namespace PlatyVGCore
{
	Device::Device()
	{
		IsZoomEnabled = false;
		IsOutsideDown = false;
		IsGlobalZoomEnabled = false;
		InputFlag = 0;
		InputLocked = -1;
		InputSensitivity = 0;
		InputBaseDistanceByMS = 0;

		CurrentSpeedyForce = 0;
		OldXForSpeedyForce = 0;
		OldYForSpeedyForce = 0;
		OldMSForSpeedyForce = 0;
	}

	Device::~Device()
	{
	}

	float Device::GetSpeedyForce(float x, float y, sint64 ms)
	{
		float Result = 1;
		if(0 < InputBaseDistanceByMS)
		{
			const float DistX = x - OldXForSpeedyForce;
			const float DistY = y - OldYForSpeedyForce;
			const float Dist = Math::Sqrt(DistX * DistX + DistY * DistY);
            const float DistTime = Math::MaxF(50, ms - OldMSForSpeedyForce);
			const float DistByMS = Math::MaxF(InputBaseDistanceByMS, Dist / DistTime);
			const float NewSpeedyForce = InputBaseDistanceByMS / DistByMS;
            CurrentSpeedyForce = NewSpeedyForce * 0.10f + CurrentSpeedyForce * 0.90f;
			Result = Math::MaxF(0, Math::MinF(CurrentSpeedyForce, 1));
		}
		OldXForSpeedyForce = x;
		OldYForSpeedyForce = y;
		OldMSForSpeedyForce = ms;
		return Result;
	}
}
