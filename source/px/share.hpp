#pragma once
#include "../px/buffer.hpp"

namespace Px
{
	//! \brief 공유자원관리
	class Share
	{
	public:
        static const Share* Create(buffer gift);
		static const Share* Create(const buffer& sample, sint32 mincount);
		static const Share* CopyOnWrite(const Share* share, sint32 mincount);
		static void Remove(const Share* share);
		static void DebugPrint();

	public:
		chars Type() const;
		const Share* Clone() const;
		sint32 Count() const;
		template<typename TYPE>
		TYPE& At(sint32 index) const
		{return Buffer::At<TYPE>(data, index);}
		buffer CopiedBuffer() const;

	private:
		Share(buffer gift);
		Share(const buffer& sample, sint32 mincount);
		Share(const Share& rhs, sint32 mincount);
		~Share();

	private:
		static sint32& _DebugShareCount();

	private:
		mutable sint32 sharecount;
		sint32 validcount;
		buffer data;
	};
}
