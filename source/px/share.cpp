#include "share.hpp"
#include "../px/string.hpp"

namespace Px
{
	const Share* Share::Create(buffer gift)
	{
		return new Share(gift);
	}

	const Share* Share::Create(const buffer& sample, sint32 mincount)
	{
		return new Share(sample, mincount);
	}

	const Share* Share::CopyOnWrite(const Share* share, sint32 mincount)
	{
		if(share)
		{
			if(1 < share->sharecount) // 독립
			{
				--_DebugShareCount();
				--share->sharecount;
				share = new Share(*share, mincount);
			}
			if(share->validcount < mincount) // 확장
			{
				Share* wshare = (Share*) share;
				wshare->validcount = mincount;
				sint32 count = Buffer::CountOf(share->data);
				while(count < mincount) count <<= 1;
				wshare->data = Buffer::Realloc(NEEDS share->data, count);
			}
			else if(mincount == -1 && 0 < share->validcount) // 후미삭제
			{
				Share* wshare = (Share*) share;
				Buffer::Reset(wshare->data, --wshare->validcount);
			}
		}
		return share;
	}

	void Share::Remove(const Share* share)
	{
		if(share)
		{
			--_DebugShareCount();
			if(--share->sharecount == 0)
				delete share;
		}
	}

	void Share::DebugPrint()
	{
		PX_TRACE("<>:");
		PX_TRACE("<>:==================== Share ====================");
		PX_TRACE("<>:>>>> <A> EA", BxARG(_DebugShareCount()));
		PX_TRACE("<>:===============================================");
	}

	chars Share::Type() const
	{
		return Buffer::NameOf(data);
	}

	const Share* Share::Clone() const
	{
		++_DebugShareCount();
		++sharecount;
		return this;
	}

	sint32 Share::Count() const
	{
		return validcount;
	}

	buffer Share::CopiedBuffer() const
	{
		const sint32 Count = Buffer::CountOf(data);
		buffer NewBuffer = Buffer::AllocBySample(NEEDS Count, data);
		Buffer::Copy(NewBuffer, 0, data, 0, Count);
		return NewBuffer;
	}

	Share::Share(buffer gift)
	{
		++_DebugShareCount();
		sharecount = 1;
		validcount = 0;
		data = gift;
	}

	Share::Share(const buffer& sample, sint32 mincount)
	{
		++_DebugShareCount();
		sharecount = 1;
		validcount = 0;
		data = Buffer::AllocBySample(NEEDS mincount, sample);
	}

	Share::Share(const Share& rhs, sint32 mincount)
	{
		++_DebugShareCount();
		sharecount = 1;
		validcount = rhs.validcount;
		const sint32 rhscount = Buffer::CountOf(rhs.data);
		const sint32 maxcount = (rhscount > mincount)? rhscount : mincount;
		data = Buffer::AllocBySample(NEEDS maxcount, rhs.data);
		Buffer::Copy(data, 0, rhs.data, 0, validcount);
	}

	Share::~Share()
	{
		Buffer::Free(data);
	}

	sint32& Share::_DebugShareCount()
	{
		static sint32 ShareCount = 0;
		return ShareCount;
	}
}
