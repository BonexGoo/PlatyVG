#pragma once
#include <platyvg.hpp>
#include "../element/coord.hpp"

namespace PlatyVGApi
{
	//! \brief 입력의 좌표화
	class Input
	{
	public:
		/*!
		\brief 스크린 다운입력
		\param id : 멀티터치ID
		\param x : 스크린좌표X
		\param y : 스크린좌표Y
		\param force : 압력값(0.0에서 1.0까지)
		\param deviceby : 장치타입
		\see ScreenDrag, ScreenUp
		*/
		static void ScreenDown(uint32 id, float x, float y, float force, const DeviceBy deviceby = DeviceByTouch);

		/*!
		\brief 스크린 드래그입력
		\param id : 멀티터치ID
		\param x : 스크린좌표X
		\param y : 스크린좌표Y
		\param force : 압력값(0.0에서 1.0까지)
		\param deviceby : 장치타입
		\see ScreenDown, ScreenUp
		*/
		static void ScreenDrag(uint32 id, float x, float y, float force, const DeviceBy deviceby = DeviceByTouch);

		/*!
		\brief 스크린 업입력
		\param id : 멀티터치ID
		\param deviceby : 장치타입
		\see ScreenDown, ScreenDrag
		*/
		static void ScreenUp(uint32 id, const DeviceBy deviceby = DeviceByTouch);

		/*!
		\brief 캔버스 다운입력
		\param x : 캔버스좌표X
		\param y : 캔버스좌표Y
		\param force : 압력값(0.0에서 1.0까지)
		\param deviceby : 장치타입
		\see PenDrag, PenUp
		*/
		static void CanvasDown(float x, float y, float force, const DeviceBy deviceby = DeviceByPen);

		/*!
		\brief 캔버스 드래그입력
		\param x : 캔버스좌표X
		\param y : 캔버스좌표Y
		\param force : 압력값(0.0에서 1.0까지)
		\param deviceby : 장치타입
		\see PenDown, PenUp
		*/
		static void CanvasDrag(float x, float y, float force, const DeviceBy deviceby = DeviceByPen);

		/*!
		\brief 캔버스 업입력
		\param deviceby : 장치타입
		\see PenDown, PenDrag
		*/
		static void CanvasUp(const DeviceBy deviceby = DeviceByPen);

		/*!
		\brief 장치별 민감도 설정
		\param deviceby : 장치타입
		\param sensitivity : 민감도값(0.0에서 1.0까지)
		*/
		static void SetDeviceSensitivity(const DeviceBy deviceby, float sensitivity);

		/*!
		\brief 장치별 민감도 얻기
		\param deviceby : 장치타입
		\return 민감도값(0.0에서 1.0까지)
		*/
		static const float GetDeviceSensitivity(const DeviceBy deviceby);

		/*!
		\brief 장치별 스피디포스 설정
		\param deviceby : 장치타입
		\param distancebysecond : 1초당 기준거리
		*/
		static void SetDeviceSpeedyForceOn(const DeviceBy deviceby, float distancebysecond);

		/*!
		\brief 장치별 스피디포스 해제
		\param deviceby : 장치타입
		*/
		static void SetDeviceSpeedyForceOff(const DeviceBy deviceby);

		/*!
		\brief 장치별 레이어설정
		\param deviceby : 장치타입
		\param layer : 레이어번호
		*/
		static void SetDeviceLayer(const DeviceBy deviceby, const sint32 layer);

		/*!
		\brief 장치별 레이어얻기
		\param deviceby : 장치타입
		\return 레이어번호
		*/
		static const sint32 GetDeviceLayer(const DeviceBy deviceby);

		/*!
		\brief 장치별 터치좌표 수량얻기
		\param deviceby : 장치타입
		\return 터치좌표 수량
		*/
		static const sint32 GetDeviceCoordCount(const DeviceBy deviceby);

		/*!
		\brief 장치별 터치좌표 배열얻기
		\param deviceby : 장치타입
		\return 터치좌표 배열
		*/
		static const float* GetDeviceCoordArray(const DeviceBy deviceby);
	};
}
