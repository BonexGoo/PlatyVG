#pragma once
#include "../px/share.hpp"

namespace Px
{
	class ArrayShare
	{
	public:
		ArrayShare(const Share* share);
		~ArrayShare();
		const Share* Clone() const;

	private:
		const Share* share;
	};

	//! \brief 스마트배열지원
	template<typename TYPE, bool CANCOPY = false, int MINCOUNT = 4>
	class Array
	{
	public:
		/*!
		\brief 생성
		*/
        Array() : share(Share::Create(_Sample(), MINCOUNT)) {}

		/*!
		\brief 생성
		\param rhs : 복사할 인스턴스
		*/
		Array(const Array& rhs) : share(rhs.share->Clone()) {}

		/*!
		\brief 생성(공유객체로부터)
		\param rhs : 복사할 인스턴스
		*/
		Array(const ArrayShare& rhs) : share(rhs.Clone()) {}

		/*!
		\brief 소멸
		*/
		~Array() {Share::Remove(share);}

		/*!
		\brief 비우기
		*/
		void Clear()
		{
			Share::Remove(share);
            share = Share::Create(_Sample(), MINCOUNT);
		}

		/*!
		\brief 공유객체 생성
		\return 공유객체
		*/
		operator const ArrayShare() const {return ArrayShare(share);}

		/*!
		\brief 복사
		\param rhs : 복사할 인스턴스
		\return 자기 객체
		*/
		Array& operator=(const Array& rhs)
		{
			Share::Remove(share);
			share = rhs.share->Clone();
			return *this;
		}

		/*!
		\brief 읽기접근
		\param index : 인덱스(음수는 역인덱스)
		\return 해당 객체
		*/
		const TYPE& operator[](sint32 index) const
		{
			if(index < 0) index = share->Count() + index;
			PX_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index && index < share->Count());
			return share->At<TYPE>(index);
		}

		/*!
		\brief 쓰기접근
		\param index : 인덱스(음수는 역인덱스)
		\return 해당 객체
		*/
		TYPE& At(sint32 index)
		{
			if(index < 0) index = share->Count() + index;
			PX_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index && index < share->Count());
			share = Share::CopyOnWrite(share, 0);
			return share->At<TYPE>(index);
		}

		/*!
		\brief 추가(후미+1)하여 쓰기접근
		\return 해당 객체
		*/
		TYPE& AtAdding()
		{
			const sint32 index = share->Count();
			share = Share::CopyOnWrite(share, index + 1);
			return share->At<TYPE>(index);
		}

		/*!
		\brief 자율추가하여 쓰기접근
		\param index : 인덱스
		\return 해당 객체
		*/
		TYPE& AtWherever(sint32 index)
		{
			share = Share::CopyOnWrite(share, index + 1);
			return share->At<TYPE>(index);
		}

		/*!
		\brief 삭제(후미-1)
		\return 삭제 수행여부(배열수량이 0일 경우 false)
		*/
		bool Subtraction()
		{
			if(!share->Count()) return false;
			share = Share::CopyOnWrite(share, -1);
			return true;
		}

		/*!
		\brief 타입명 구하기
		\return 타입명
		*/
		chars Type() const
		{
			return share->Type();
		}

		/*!
		\brief 배열수량 구하기
		\return 배열수량
		*/
		sint32 Count() const
		{
			return share->Count();
		}

	private:
		const buffer& _Sample()
		{
			static const buffer Sample = Buffer::Alloc<TYPE>(NEEDS 0, CANCOPY);
			return Sample;
		}

	private:
		const Share* share;
	};

	//! \brief 스마트배열 기본객체
	typedef Array<sint08, true> sint08s;
	typedef Array<uint08, true> uint08s;
	typedef Array<sint16, true> sint16s;
	typedef Array<uint16, true> uint16s;
	typedef Array<sint32, true> sint32s;
	typedef Array<uint32, true> uint32s;
	typedef Array<sint64, true> sint64s;
	typedef Array<uint64, true> uint64s;
	typedef Array<float, true> floats;

	//! \brief 공용객체지원
	template<typename TYPE>
	class Union
	{
	public:
		Union() : share(nullptr) {}
		Union(const Union& rhs) : share(rhs.share->Clone()) {}
		~Union() {Share::Remove(share);}

		Union& operator=(const Union& rhs)
		{
			Share::Remove(share);
			share = rhs.share->Clone();
			return *this;
		}

		Union& operator=(buffer gift)
		{
			Share::Remove(share);
			share = Share::Create(gift);
			return *this;
		}

		TYPE* operator->()
		{
			PX_ASSERT("빈 객체입니다", share);
			return ptr();
		}

		TYPE const* operator->() const
		{
			PX_ASSERT("빈 객체입니다", share);
			return ptr_const();
		}

		TYPE* ptr()
		{
			if(!share) return nullptr;
			return &share->At<TYPE>(0);
		}

		TYPE const* ptr_const() const
		{
			if(!share) return nullptr;
			return &share->At<TYPE>(0);
		}

		buffer copied_buffer() const
		{
			PX_ASSERT("빈 객체의 버퍼복사를 시도합니다", share);
			return share->CopiedBuffer();
		}

	private:
		const Share* share;
	};
}
