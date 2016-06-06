#pragma once
#include <platyvg.hpp>
#include "../element/color.hpp"
#include "../element/rect.hpp"
#include "../element/size.hpp"
#include "../element/zoom.hpp"

namespace PlatyVGApi
{
	//! \brief 명령입력
	class Command
	{
	public:
		class Canvas
		{
		public:
			/*!
			\brief 자료 생성하기
			\param focusable : 포커싱가능여부
			\return 자료의 ID
			*/
			static const sint32 Create(const bool focusable);

			/*!
			\brief 해당 자료를 제거하기
			\param doc : 자료의 ID
			*/
			static void Remove(const sint32 doc);

			/*!
			\brief 해당 자료를 업데이트하기
			\param doc : 자료의 ID
			*/
			static void Update(const sint32 doc);

			/*!
			\brief 해당 자료를 해당 반지름길이로 업데이트하기
			\param doc : 자료의 ID
			\param radius : 반지름길이(픽셀단위)
			*/
			static void UpdateBy(const sint32 doc, const float radius);

			/*!
			\brief 해당 자료의 이름설정
			\param doc : 자료의 ID
			\param name : 자료의 이름
			*/
            static void SetName(const sint32 doc, chars name);

			/*!
			\brief 해당 자료의 이름얻기
			\param doc : 자료의 ID
			\return 자료의 이름
			*/
			static const String GetName(const sint32 doc);

			/*!
			\brief 해당 자료의 영역설정
			\param doc : 자료의 ID
			\param rect : 영역값
			*/
			static void SetArea(const sint32 doc, const Rect rect);

			/*!
			\brief 해당 자료의 영역얻기
			\param doc : 자료의 ID
			\return 영역값
			*/
			static const Rect GetArea(const sint32 doc);

			/*!
			\brief 해당 자료를 줌설정
			\param doc : 자료의 ID
			\param zoom : 줌값
			*/
			static void SetZoom(const sint32 doc, const Zoom zoom);

			/*!
			\brief 해당 자료를 특정 스크린영역으로 재배치
			\param doc : 자료의 ID
			\param rect : 영역값
			\param boundby : 바운드규칙
			*/
			static void SetZoomInRect(const sint32 doc, const Rect rect, const BoundBy boundby = BoundByInner);

			/*!
			\brief 해당 자료의 줌얻기
			\param doc : 자료의 ID
			\return 줌값
			*/
			static const Zoom GetZoom(const sint32 doc);

			/*!
			\brief 해당 자료를 포커싱
			\param doc : 자료의 ID
			\param popup : 팝업 여부
			*/
            static void SetFocus(const sint32 doc, const bool popup = true);

			/*!
			\brief 포커싱된 자료얻기
			\return 자료의 ID(포커싱된 자료가 없으면 -1)
			*/
			static const sint32 GetFocus();

			/*!
			\brief 자료의 수량얻기
			\return 자료의 수량
			*/
			static const sint32 GetDocumentCount();

			/*!
			\brief 해당 출력순번의 자료얻기
			\param index : 해당 출력순번
			\return 자료의 ID(위치된 자료가 없으면 -1)
			*/
			static const sint32 GetDocument(const sint32 index);

			/*!
			\brief 해당 위치의 자료얻기
			\param point : 해당 위치
			\return 자료의 ID(위치된 자료가 없으면 -1)
			*/
			static const sint32 GetDocumentBy(const Point point);

			/*!
			\brief 해당 자료의 마지막 그려진 시간얻기
			\param doc : 자료의 ID
			\return 밀리초시간(저장후 그려진 내용이 없으면 -1)
			*/
			static const sint64 GetTimeOfLastDrawing(const sint32 doc);

			/*!
			\brief 해당 자료의 스냅샷얻기
			\param doc : 자료의 ID
			\param bits : 비트맵픽셀정보 시작포인터
			\param width : 비트맵의 가로길이
			\param height : 비트맵의 세로길이
			\param boundby : 바운드규칙
			*/
			static void Snapshot(const sint32 doc, uint32* bits, const sint32 width, const sint32 height, const BoundBy boundby = BoundByInner);
		};

		/*!
		\brief 해당 자료의 도형의 수량얻기
		\param doc : 자료의 ID
		\param layer : 레이어번호(-1은 전체레이어)
		\return 도형의 수량
		*/
		static sint32 GetShapeCount(const sint32 doc, const sint32 layer = -1);

		/*!
		\brief 해당 자료의 도형 초기화
		\param doc : 자료의 ID
		\param layer : 레이어번호(-1은 전체레이어)
		*/
		static void ClearShapes(const sint32 doc, const sint32 layer = -1);

		/*!
		\brief 현재 자료의 선택 초기화
		*/
		static void ClearSelects();

		/*!
		\brief 현재 자료의 선택된 도형을 제거
		*/
		static void RemoveSelects();

		/*!
		\brief 현재 자료의 선택된 도형을 색상변경
		\param color : 색상값
		\param changecolorby : 색상변경규칙
		*/
		static void ChangeSelects(const Color color, const ChangeColorBy changecolorby);

		/*!
		\brief 현재 스트로크설정
		\param name : 스트로크명("spline", "tube", "aqua", "erase", "select", "hand", "spoid")
		*/
		static void SetStrokeBy(chars name);

		/*!
		\brief 현재 선택모드설정
		\param name : 선택모드명("rect", "drag", "area")
		*/
		static void SetSelectBy(chars name);

		/*!
		\brief 현재 색상설정
		\param color : 색상값
		*/
		static void SetColor(const Color color);

		/*!
		\brief 현재 색상얻기
		\return 색상값
		*/
		static const Color GetColor();

		/*!
		\brief 해당 위치의 색상얻기
		\param point : 해당 위치
		\param backcolor : 배경색
		\return 색상값
		*/
		static const Color GetColorBy(const Point point, const Color backcolor);

		/*!
		\brief 현재 두께설정
		\param thick : 두께값
		*/
		static void SetThick(const float thick);

		/*!
		\brief 현재 두께얻기
		\return 두께값
		*/
		static const float GetThick();

		/*!
		\brief 뷰의 중점기준 반지름길이설정
		\param radius : 반지름길이(픽셀단위)
		\param updateAllDocuments : 업데이트 여부
		*/
        static void SetViewRadius(const float radius, const bool updateAllDocuments = true);

		/*!
		\brief 뷰의 중점기준 반지름길이얻기
		\return 반지름길이(픽셀단위)
		*/
		static const float GetViewRadius();

		/*!
		\brief 해당 레이어의 보여짐설정
		\param layer : 레이어번호
		\param visible : 보여짐 여부
		*/
        static void SetLayerVisible(const sint32 layer, const bool visible);

		/*!
		\brief 해당 레이어의 보여짐얻기
		\param layer : 레이어번호
		\return 보여짐 여부
		*/
        static const bool GetLayerVisible(const sint32 layer);

		/*!
		\brief 현재 시간설정
		\param ms : 밀리초시간
		*/
		static void SetCurrentTime(const sint64 ms);

		/*!
		\brief 현재 자료 실행취소
		\return 수행여부
		*/
        static bool Undo();

		/*!
		\brief 현재 자료 재실행
		\return 수행여부
		*/
        static bool Redo();

		/*!
		\brief 현재 자료의 실행취소 가능여부
		\return 가능여부
		*/
        static bool CanUndo();

		/*!
		\brief 현재 자료의 재실행 가능여부
		\return 가능여부
		*/
        static bool CanRedo();
	};
}
