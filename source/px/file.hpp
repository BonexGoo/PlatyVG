#pragma once
#include "../px/px.hpp"

namespace Px
{
	//! \brief 파일함수지원
	class File
	{
	public:
		/*!
		\brief 파일열기(읽기전용)
		\param filename : 파일명
		\return 파일포인터
		\see Close
		*/
		static const void* OpenForRead(chars filename);

		/*!
		\brief 파일열기(쓰기전용)
		\param filename : 파일명
		\return 파일포인터
		\see Close
		*/
		static void* OpenForWrite(chars filename);

		/*!
		\brief 파일닫기
		\param file : 파일포인터
		\see OpenForRead, OpenForWrite
		*/
		static void Close(const void* file);

		/*!
		\brief 파일사이즈 얻기
		\param file : 파일포인터
		\return 파일사이즈
		*/
		static const sint32 GetSize(const void* file);

		/*!
		\brief 읽기
		\param file : 파일포인터
		\param data : 저장할 데이터
		\param size : 데이터의 길이(byte)
		\see OpenForRead
		*/
		static void Read(const void* file, uint08* data, const sint32 size);

		/*!
		\brief 쓰기
		\param file : 파일포인터
		\param data : 불러올 데이터
		\param size : 데이터의 길이(byte)
		\see OpenForWrite
		*/
		static void Write(void* file, bytes data, const sint32 size);

		/*!
		\brief 구간통과
		\param file : 파일포인터
		\param size : 통과할 길이(byte)
		\see OpenForRead, OpenForWrite
		*/
		static void Skip(const void* file, const sint32 size);
	};
}
