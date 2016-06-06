#pragma once
#include <platyvg.hpp>

namespace PlatyVGApi
{
	//! \brief 자료의 포맷화
	class Export
	{
	public:
		/*!
		\brief 해당 자료에 파일 불러오기
		\param doc : 자료의 ID
		\param filename : 파일명
		\param usingArea : Area의 적용여부
		\param usingZoom : Zoom의 적용여부
		\return 다음 파일의 필요여부
		*/
        static bool Load(const sint32 doc, chars filename, const bool usingArea, const bool usingZoom);

		/*!
		\brief 해당 자료에 도형 불러오기
		\param doc : 자료의 ID
		\param layer : 적용할 레이어번호
		\param edgebuffer : 도형버퍼
		\param buffersize : 버퍼길이
		\return 성공여부
		*/
		static bool LoadShape(const sint32 doc, const sint32 layer, bytes shapebuffer, sint32 buffersize);

		/*!
		\brief 해당 자료에 VG파일 불러오기
		\param doc : 자료의 ID
		\param layer : 적용할 레이어번호
		\param filename : 파일명
		\return 성공여부
		*/
        static bool LoadVG(const sint32 doc, const sint32 layer, chars filename);

		/*!
		\brief 해당 자료를 파일로 저장
		\param doc : 자료의 ID
		\param filename : 파일명
		\param shapebegin : 도형의 시작번호
		\return 다음 파일의 시작번호(완료된 경우 0)
		*/
		static sint32 Save(const sint32 doc, chars filename, const sint32 shapebegin);

		/*!
		\brief 파일에 스키닝첨가
		\param filename : 파일명
		\param bmpFilename : 스킨을 입힐 비트맵파일명
		\return 성공여부
		*/
        static bool MergeSkin(chars filename, chars bmpFilename);
	};
}
