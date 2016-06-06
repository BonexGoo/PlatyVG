#pragma once
#include "../px/array.hpp"

#define CONST_STRING(STR) Px::String::_Const<__COUNTER__ * 10000 + __LINE__>(STR)

namespace Px
{
	//! \brief 스트링지원
	class String
	{
	public:
		/*!
		\brief 생성
		*/
		String();

		/*!
		\brief 생성
		\param rhs : 복사할 인스턴스
		*/
		String(const String& rhs);

		/*!
		\brief 생성(스트링으로부터)
		\param rhs : 복사할 스트링
		*/
		String(chars rhs);

		/*!
		\brief 생성(스트링으로부터)
		\param rhs : 복사할 스트링
		\param length : 복사할 길이
		*/
		String(chars rhs, sint32 length);

		/*!
		\brief 소멸
		*/
		~String();

		/*!
		\brief 복사
		\param rhs : 복사할 인스턴스
		\return 자기 객체
		*/
		String& operator=(const String& rhs);

		/*!
		\brief 복사(chars)
		\param rhs : 복사할 스트링
		\return 자기 객체
		*/
		String& operator=(chars rhs);

		/*!
		\brief 동등 비교
		\param rhs : 비교할 인스턴스
		\return 같으면 true, 다르면 false
		*/
		bool operator==(const String& rhs) const;

		/*!
		\brief 동등 비교(chars)
		\param rhs : 비교할 스트링
		\return 같으면 true, 다르면 false
		*/
		bool operator==(chars rhs) const;

		/*!
		\brief 차등 비교
		\param rhs : 비교할 인스턴스
		\return 다르면 true, 같으면 false
		*/
		bool operator!=(const String& rhs) const;

		/*!
		\brief 차등 비교(chars)
		\param rhs : 비교할 스트링
		\return 다르면 true, 같으면 false
		*/
		bool operator!=(chars rhs) const;

		/*!
		\brief 형변환
		\return chars형 스트링
		*/
		operator chars() const;

		/*!
		\brief 비우기
		*/
		void Empty();

		/*!
		\brief 접근
		\param index : 인덱스(음수는 역인덱스)
		\return 해당 객체
		*/
		const sint08 operator[](sint32 index) const;

		/*!
		\brief 텍스트길이 구하기
		\return 텍스트길이
		*/
		sint32 Length() const;

		/*!
		\brief memcmp식 비교
		\param other : 비교할 인스턴스
		\param offset : other의 시작주소
		\return 비교결과(0-같음)
		*/
		sint32 Compare(const String& other, sint32 offset) const;

	public:
		/*!
		\brief 상수객체 구하기, CONST(STR)를 이용
		\param rhs : 상수 스트링
		\return 상수객체
		*/
		template<int UNIQUE_VALUE>
		static const String& _Const(chars rhs)
		{
			static const String ConstString(rhs);
			return ConstString;
		}

	private:
		sint08s words;
	};
}
