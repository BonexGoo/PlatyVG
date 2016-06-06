#pragma once
#include <platyvg.hpp>
#include "../element/action.hpp"

namespace PlatyVGCore
{
	class Document;

	//! \brief 작업관리
	class Record
	{
	public:
		Record();
		~Record();

	public:
		/*!
		\brief 실행등록
		\param doc : 적용할 자료
		*/
		static void Commit(Document* doc, buffer action);

		/*!
		\brief 실행취소
		\param doc : 적용할 자료
		\return 수행후 실행포커스(0 ~ Max)
		*/
		static sint32 Undo(Document* doc);

		/*!
		\brief 재실행
		\param doc : 적용할 자료
		\return 수행후 실행포커스(0 ~ Max)
		*/
		static sint32 Redo(Document* doc);

		/*!
		\brief 작업포커스 가져오기
		\param doc : 적용할 자료
		\return 작업포커스
		*/
		static sint32 GetFocus(Document* doc);

		/*!
		\brief 작업수량 가져오기
		\param doc : 적용할 자료
		\return 작업수량
		*/
		static sint32 GetCount(Document* doc);

		/*!
		\brief 작업수량 최대값 설정하기
		\param doc : 적용할 자료
		\param value : 최대값
		*/
		static void SetCountMax(Document* doc, sint32 value);

		/*!
		\brief 작업수량 최대값 가져오기
		\param doc : 적용할 자료
		\return 작업수량 최대값
		*/
		static sint32 GetCountMax(Document* doc);

	public:
		UnionActions AllActions;
		sint32 ActionFocus;
		sint32 ActionLength;
		sint32 ActionLengthMax;
	};
}
