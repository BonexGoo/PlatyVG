#pragma once
#include <platyvg.hpp>
#include "../element/color.hpp"
#include "../element/coord.hpp"
#include "../element/matrix.hpp"
#include "../element/mesh.hpp"
#include "../element/zoom.hpp"

namespace PlatyVGApi
{
	//! \brief 네이티브 랜더러 연결
	class Adapter
	{
	public:
		/*!
		\brief 자료의 링크얻기
		\param doc : 자료의 ID
		\return 자료의 링크
		*/
		static linkstep1 GetDocumentLink(const sint32 doc);

		/*!
		\brief 레이어의 수량얻기
		\param doclink : 자료의 링크
		\return 레이어의 수량
		*/
		static const sint32 GetLayerCount(linkstep1 doclink);

		/*!
		\brief 레이어의 링크얻기
		\param doclink : 자료의 링크
		\param index : 레이어순번
		\return 레이어의 링크
		*/
		static linkstep2 GetLayerLink(linkstep1 doclink, const sint32 index);

		/*!
		\brief 도형의 수량얻기
		\param layerlink : 레이어의 링크
		\param dyn : 동적도형 여부
		\return 도형의 수량
		*/
		static const sint32 GetShapeCount(linkstep2 layerlink, const bool dyn);

		/*!
		\brief 도형의 링크얻기
		\param layerlink : 레이어의 링크
		\param dyn : 동적도형 여부
		\param index : 도형순번
		\return 도형의 링크
		*/
		static linkstep3 GetShapeLink(linkstep2 layerlink, const bool dyn, const sint32 index);

		/*!
		\brief 해당 레이어의 보여짐여부
		\param layerlink : 레이어의 링크
		\return 보여지면 true
		*/
		static const bool IsLayerVisibled(linkstep2 layerlink);

		/*!
		\brief 해당 도형의 선택됨여부
		\param shapelink : 도형의 링크
		\return 선택되었으면 true
		*/
		static const bool IsShapeSelected(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 컬러 Red값 얻기
		\param shapelink : 도형의 링크
		\return Red값(0에서 255까지)
		*/
		static const sint32 GetShapeColorRed(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 컬러 Green값 얻기
		\param shapelink : 도형의 링크
		\return Green값(0에서 255까지)
		*/
		static const sint32 GetShapeColorGreen(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 컬러 Blue값 얻기
		\param shapelink : 도형의 링크
		\return Blue값(0에서 255까지)
		*/
		static const sint32 GetShapeColorBlue(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 컬러 Alpha값 얻기
		\param shapelink : 도형의 링크
		\return Alpha값(0에서 255까지)
		*/
		static const sint32 GetShapeColorAlpha(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 컬러 Aqua값 얻기
		\param shapelink : 도형의 링크
		\return Aqua값(0에서 255까지)
		*/
		static const sint32 GetShapeColorAqua(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 행렬 M11값 얻기
		\param shapelink : 도형의 링크
		\return M11값
		*/
		static const float GetShapeMatrixM11(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 행렬 M12값 얻기
		\param shapelink : 도형의 링크
		\return M12값
		*/
		static const float GetShapeMatrixM12(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 행렬 M21값 얻기
		\param shapelink : 도형의 링크
		\return M21값
		*/
		static const float GetShapeMatrixM21(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 행렬 M22값 얻기
		\param shapelink : 도형의 링크
		\return M22값
		*/
		static const float GetShapeMatrixM22(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 행렬 Dx값 얻기
		\param shapelink : 도형의 링크
		\return Dx값
		*/
		static const float GetShapeMatrixDx(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 행렬 Dy값 얻기
		\param shapelink : 도형의 링크
		\return Dy값
		*/
		static const float GetShapeMatrixDy(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 메시좌표 타입얻기
		\param shapelink : 도형의 링크
		\return 메시좌표 타입
		*/
		static chars GetShapeMeshType(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 메시좌표 수량얻기
		\param shapelink : 도형의 링크
		\return 메시좌표 수량
		*/
		static const sint32 GetShapeMeshCount(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 메시좌표 배열얻기
		\param shapelink : 도형의 링크
		\return 메시좌표 배열
		*/
		static const float* GetShapeMeshArray(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 도형버퍼 길이얻기
		\param shapelink : 도형의 링크
		\return 도형버퍼 길이
		*/
		static const sint32 GetShapeBufferSize(linkstep3 shapelink);

		/*!
		\brief 해당 도형의 도형버퍼 얻기
		\param shapelink : 도형의 링크
		\return 도형버퍼
		*/
		static bytes GetShapeBuffer(linkstep3 shapelink);

		/*!
		\brief 비트맵출력
		\param doc : 자료의 ID
		\param zoom : 줌값
		\param bits : 비트맵픽셀정보 시작포인터
		\param width : 비트맵의 가로길이
		\param height : 비트맵의 세로길이
		*/
		static void RenderDIB(const sint32 doc, const Zoom zoom, uint32* bits, const sint32 width, const sint32 height);
	};
}
