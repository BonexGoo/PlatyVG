#pragma once
#include <platyvg.hpp>
#include "../core/device.hpp"
#include "../core/document.hpp"
#include "../core/shape.hpp"
#include "../element/context.hpp"
#include "../element/coord.hpp"
#include "../element/point.hpp"
#include "../element/size.hpp"

namespace PlatyVGCore
{
	//! \brief 화면관리
	class View
	{
	public:
		View();
		~View();

	public:
		/*!
		\brief 현재 뷰에 접근
		\return 현재 뷰
		*/
		static View* Currently();

		/*!
		\brief 해당 자료의 포인터 얻기
		\param doc : 자료의 ID
		\return 자료의 포인터
		*/
		Document* GetDocument(sint32 doc);

		/*!
		\brief 작업할 동적도형 확보
		\param doc : 대상 자료
		\param deviceby : 입력타입
		\param strokeby : 도형타입
		*/
		void LockDynShape(Document* doc, const DeviceBy deviceby, const StrokeBy strokeby);

		/*!
		\brief 작업한 동적도형 반환
		\param doc : 대상 자료
		\param deviceby : 입력타입
		\param addition : 도형리스트에 추가요청
		\return 추가되었는지의 여부
		*/
		bool UnlockDynShape(Document* doc, const DeviceBy deviceby, const bool addition);

		/*!
		\brief 확보된 동적도형 얻기
		\param doc : 대상 자료
		\param deviceby : 입력타입
		\return 동적도형
		*/
		Shape* GetDynShape(Document* doc, const DeviceBy deviceby);

		/*!
		\brief 민감도를 적용한 좌표추가
		\param deviceby : 입력타입
		\param x : 터치좌표X
		\param y : 터치좌표Y
		\param force : 압력값(0.0에서 1.0까지)
		*/
		void AddCoordSensitivity(const DeviceBy deviceby, float x, float y, float force);

	public:
		// Document
		Document DocumentRoot;
		Document* DocumentFocus;
		// Option
		StrokeBy OptionStrokeBy;
		SelectBy OptionSelectBy;
		Context OptionContext;
		float OptionRadius;
		sint64 OptionMS;
		// Device
		Device DeviceState[LengthOfDevice];
		sint32 DeviceLayer[LengthOfDevice];
	};
}
