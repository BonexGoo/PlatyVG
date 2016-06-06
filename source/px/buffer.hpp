#pragma once
#include "../px/memory.hpp"

namespace Px
{
	//! \brief 버퍼지원
	class Buffer
	{
	public:
		/*!
		\brief 할당
		\param count : 배열수량
		\param canCopy : Memcpy사용가능 여부
		\return 할당된 버퍼주소(형변환됨)
		\see AllocBySample, Realloc, Free
		*/
		template<typename TYPE = uint08>
		static buffer Alloc(NEEDS_DBG_PRM sint32 count, bool canCopy = false)
		{return _Alloc(NEEDS_DBG_ARG count, _Assignor<TYPE>, canCopy);}

		/*!
		\brief 참고할당(샘플과 동일한 타입으로)
		\param count : 배열수량
		\param sample : 참고되는 버퍼주소
		\return 할당된 버퍼주소
		\see Alloc, Realloc, Free
		*/
		static buffer AllocBySample(NEEDS_DBG_PRM sint32 count, const buffer sample);

		/*!
		\brief 재할당(배열수량이 변경되었을 때만)
		\param buf : 버퍼주소
		\param count : 재할당할 배열수량
		\return 재할당된 버퍼주소
		\see Alloc, AllocBySample, Free
		*/
		static buffer Realloc(NEEDS_DBG_PRM buffer buf, sint32 count);

		/*!
		\brief 해제
		\param buf : 버퍼주소
		\see Alloc, AllocBySample, Realloc
		*/
		static void Free(const buffer buf);

		/*!
		\brief 복사
		\param dstBuf : 붙여넣을 버퍼
		\param dstOff : 붙여넣을 오프셋
		\param srcBuf : 복사할 버퍼
		\param srcOff : 복사할 오프셋
		\param count : 복사할 배열수량
		*/
		static void Copy(buffer dstBuf, sint32 dstOff, const buffer srcBuf, sint32 srcOff, sint32 count);

		/*!
		\brief 배열수량 구하기
		\param buf : 버퍼주소
		\return 배열수량
		\see SizeOf, TypeOf, NameOf
		*/
		static sint32 CountOf(const buffer buf);

		/*!
		\brief 객체사이즈 구하기
		\param buf : 버퍼주소
		\return 객체사이즈
		\see CountOf, TypeOf, NameOf
		*/
		static sint32 SizeOf(const buffer buf);

		/*!
		\brief 객체타입 구하기
		\param buf : 버퍼주소
		\return 객체타입(해당 _Assignor함수의 주소)
		\see CountOf, SizeOf, NameOf
		*/
		static sblock TypeOf(const buffer buf);

		/*!
		\brief 객체이름 구하기
		\param buf : 버퍼주소
		\return 객체이름
		\see CountOf, SizeOf, TypeOf
		*/
		static chars NameOf(const buffer buf);

		/*!
		\brief 쓰기접근
		\param buf : 버퍼주소
		\param index : 인덱스
		\return 해당 객체
		*/
		template<typename TYPE = uint08>
		static TYPE& At(const buffer buf, sint32 index)
		{return *((TYPE*) _At(buf, index, _Assignor<TYPE>));}

		/*!
		\brief 초기화
		\param buf : 버퍼주소
		\param index : 초기화할 인덱스
		*/
		static void Reset(buffer buf, sint32 index);

		/*!
		\brief 잔존메모리 현황보고
		*/
		static void DebugPrint();

	private:
		typedef void (*_AssignorCB)(buffer, sblock*);
		template<typename TYPE>
		static void _Assignor(buffer buf, sblock* cmd)
		{
			enum {TypeNameLengthMax = 63};
			static sint08 TypeName[TypeNameLengthMax + 1] = {'\0'};
			static bool CanCopy = false;
			if(buf && cmd)
			{
				if(cmd[1]) // 복사
				{
					TYPE* DstData = (TYPE*) buf;
					const TYPE* SrcData = (TYPE*) cmd[0];
					const sint32 Count = (sint32) cmd[1];
					if(CanCopy)
						Memory::Copy(DstData, SrcData, sizeof(TYPE) * Count);
					else for(sint32 i = 0; i < Count; ++i)
						DstData[i] = SrcData[i];
				}
				else // 초기화
				{
					TYPE* DstData = (TYPE*) buf;
					const sint32 Index = (sint32) cmd[0];
					DstData[Index].~TYPE();
					new(PTR_TO_SBLOCK(&DstData[Index])) TYPE;
				}
			}
			else if(buf)
			{
				TYPE* Data = (TYPE*) buf;
				const sint32 Count = (sint32) buf[-2];
				if(buf[-1]) // 소멸
				{
					for(sint32 i = 0; i < Count; ++i)
						Data[i].~TYPE();
					_Free(buf);
				}
				else // 생성
					for(sint32 i = 0; i < Count; ++i)
						new(PTR_TO_SBLOCK(&Data[i])) TYPE;
			}
			else if(cmd)
			{
				// 타입사이즈
				cmd[0] = (sblock) sizeof(TYPE);
				// Memcpy사용가능 여부
				if(cmd[1]) CanCopy = true;
				// 타입명
				if(cmd[2])
				{
					if(TypeName[0] == '\0')
					{
						chars FuncFocus = nullptr;
						chars TypeNameBegin = nullptr;
						chars TypeNameEnd = nullptr;
						#ifdef WIN32
							FuncFocus = __FUNCSIG__;
							while(*FuncFocus != '<') ++FuncFocus;
							TypeNameBegin = ++FuncFocus;
							while(*FuncFocus != '>') ++FuncFocus;
							TypeNameEnd = FuncFocus;
						#else
							FuncFocus = __PRETTY_FUNCTION__;
							while(*FuncFocus != '[') ++FuncFocus;
							TypeNameBegin = ++FuncFocus;
							while(*FuncFocus != ']' && *FuncFocus != ';') ++FuncFocus;
							TypeNameEnd = FuncFocus;
						#endif
						FuncFocus = TypeNameBegin;
						while(++FuncFocus < TypeNameEnd)
							if(*FuncFocus == ' ') TypeNameBegin = FuncFocus + 1;
						const sint32 TypeNameLength = TypeNameEnd - TypeNameBegin;
						PX_ASSERT("타입명이 너무 깁니다", TypeNameLength <= TypeNameLengthMax);
						Memory::Copy(TypeName, TypeNameBegin, TypeNameLength);
						TypeName[TypeNameLength] = '\0';
					}
					cmd[2] = PTR_TO_SBLOCK(TypeName);
				}
			}
		}
		static buffer _Alloc(NEEDS_DBG_PRM sint32 count, _AssignorCB cb, bool canCopy);
		static void _Free(const buffer buf);
		static const nakedbuffer _Convert(const buffer buf);
		static sint32 _SizeOf(const nakedbuffer naked);
		static chars _NameOf(_AssignorCB cb);
		static void* _At(const buffer buf, sint32 index, _AssignorCB cb);
		static void _Mutex(bool lock);
		static void _DebugAttach(NEEDS_DBG_PRM const nakedbuffer naked, sint32 blocklen, sint32 reallen);
		static void _DebugDetach(const nakedbuffer naked);
		static sblock* _DebugLink();
	};
}
