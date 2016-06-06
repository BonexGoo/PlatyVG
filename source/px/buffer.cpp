#include "buffer.hpp"
#include "../px/math.hpp"

#ifdef _WIN32
	#include <windows.h>
	#define MUTEX_ID struct{HANDLE E; long C;}
	#define MUTEX_INIT {CreateEvent(0, 0, 0, 0), 0}
	#define MUTEX_DESTROY(ID) do{if(ID.E) {CloseHandle(ID.E); ID.E = 0;}}while(false)
	#define MUTEX_LOCK(ID) do{if(1 < InterlockedIncrement(&ID.C)) WaitForSingleObject(ID.E, INFINITE);}while(false)
	#define MUTEX_UNLOCK(ID) do{if(0 < InterlockedDecrement(&ID.C)) SetEvent(ID.E);}while(false)
#else
    #if __APPLE__ != 1
        #ifndef INCLUDED_SIZE_T
        #define INCLUDED_SIZE_T
            typedef ublock size_t;
        #endif
    #endif
	#include <pthread.h>
	#define MUTEX_ID pthread_mutex_t
	#define MUTEX_INIT PTHREAD_MUTEX_INITIALIZER
	#define MUTEX_DESTROY(ID) pthread_mutex_destroy(&ID)
	#define MUTEX_LOCK(ID) pthread_mutex_lock(&ID)
	#define MUTEX_UNLOCK(ID) pthread_mutex_unlock(&ID)
#endif

#define CALC_BLOCK_LEN(BYTES_LEN) (((BYTES_LEN) + sizeof(sblock) - 1) / sizeof(sblock))
#define CALC_BYTES_LEN(BLOCK_LEN) (sizeof(sblock) * (2 + (BLOCK_LEN) + SIZE_OF_DEBUGINFO))
#define NO_CHANGE_FLAG (false)
#define LINK_L (SIZE_OF_DEBUGINFO - 2)
#define LINK_R (SIZE_OF_DEBUGINFO - 1)
#ifdef PX_DEBUG
	#define SIZE_OF_DEBUGINFO (7)
#else
	#define SIZE_OF_DEBUGINFO (0)
#endif

namespace Px
{
	buffer Buffer::AllocBySample(NEEDS_DBG_PRM sint32 count, const buffer sample)
	{
		const nakedbuffer SampleNaked = _Convert(sample);
		_AssignorCB SampleAssignor = (_AssignorCB) SampleNaked[1];
		return _Alloc(NEEDS_DBG_ARG count, SampleAssignor, NO_CHANGE_FLAG);
	}

	buffer Buffer::Realloc(NEEDS_DBG_PRM buffer buf, sint32 count)
	{
		const nakedbuffer OldNaked = _Convert(buf);
		if(count == OldNaked[0]) return buf;
		_AssignorCB OldAssignor = (_AssignorCB) OldNaked[1];
		buffer NewBuffer = _Alloc(NEEDS_DBG_ARG count, OldAssignor, NO_CHANGE_FLAG);
		const sint32 CopyCount = Math::Min(count, OldNaked[0]);
		if(0 < CopyCount)
		{
			sblock CopyCmd[2] = {PTR_TO_SBLOCK(buf), CopyCount};
			OldAssignor(NewBuffer, CopyCmd);
		}
		Free(buf);
		return NewBuffer;
	}

	void Buffer::Free(const buffer buf)
	{
		if(!buf) return;
		const nakedbuffer OldNaked = _Convert(buf);
		((_AssignorCB) OldNaked[1])(buf, nullptr);
	}

	void Buffer::Copy(buffer dstBuf, sint32 dstOff, const buffer srcBuf, sint32 srcOff, sint32 count)
	{
		if(count <= 0) return;
		const nakedbuffer DstNaked = _Convert(dstBuf);
		const nakedbuffer SrcNaked = _Convert(srcBuf);
		PX_ASSERT("서로 다른 타입간 복사입니다", DstNaked[1] == SrcNaked[1]);
		PX_ASSERT("복사범위가 dst를 초과합니다", 0 <= dstOff && dstOff + count <= DstNaked[0]);
		PX_ASSERT("복사범위가 src를 초과합니다", 0 <= srcOff && srcOff + count <= SrcNaked[0]);
		_AssignorCB DstAssignor = (_AssignorCB) DstNaked[1];
		sblock CopyCmd[2] = {PTR_TO_SBLOCK(srcBuf) + SizeOf(srcBuf) * srcOff, count};
		DstAssignor((buffer) (PTR_TO_SBLOCK(dstBuf) + SizeOf(dstBuf) * dstOff), CopyCmd);
	}

	sint32 Buffer::CountOf(const buffer buf)
	{
		const nakedbuffer Naked = _Convert(buf);
		return (sint32) Naked[0];
	}

	sint32 Buffer::SizeOf(const buffer buf)
	{
		return (sint32) _SizeOf(_Convert(buf));
	}

	sblock Buffer::TypeOf(const buffer buf)
	{
		const nakedbuffer Naked = _Convert(buf);
		return Naked[1];
	}

	chars Buffer::NameOf(const buffer buf)
	{
		const nakedbuffer Naked = _Convert(buf);
		return _NameOf((_AssignorCB) Naked[1]);
	}

	void Buffer::Reset(buffer buf, sint32 index)
	{
		const nakedbuffer Naked = _Convert(buf);
		_AssignorCB Assignor = (_AssignorCB) Naked[1];
		sblock ClearCmd[2] = {index, 0};
		Assignor(buf, ClearCmd);
	}

	void Buffer::DebugPrint()
	{
		#ifdef PX_DEBUG
			_Mutex(true);
				const sblock* CurDebugInfo = _DebugLink();
				PX_TRACE("<>:");
				PX_TRACE("<>:==================== Buffer ====================");
                sint32 TotalBytes = 0, TotalCount = 0;
				while((CurDebugInfo = (const sblock*) CurDebugInfo[LINK_R]) != _DebugLink())
                {
                    TotalBytes += (sint32) CurDebugInfo[1];
                    TotalCount++;
					PX_TRACE("<>:>>>> <A:8> BYTES(<A>)     .....<A>(<A> Ln)",
                        BxARG((sint32) CurDebugInfo[1], (string) CurDebugInfo[4], (string) CurDebugInfo[2], (sint32) CurDebugInfo[3]));
                }
                PX_TRACE("<>:================================================");
                PX_TRACE("<>:>>>> <A:8> BYTES(<A> ea)", BxARG(TotalBytes, TotalCount));
				PX_TRACE("<>:================================================");
			_Mutex(false);
		#endif
	}

	buffer Buffer::_Alloc(NEEDS_DBG_PRM sint32 count, _AssignorCB cb, bool canCopy)
	{
		sblock InitCmd[3] = {0, canCopy, 0};
		cb(nullptr, InitCmd);
		const sint32 RealLength = InitCmd[0] * count;
		const sint32 BlockLength = CALC_BLOCK_LEN(RealLength);
		const sint32 BytesLength = CALC_BYTES_LEN(BlockLength);
		nakedbuffer NewNaked = (nakedbuffer) Memory::Alloc(BytesLength);
		NewNaked[0] = count;
		NewNaked[1] = 0;
		cb((buffer) &NewNaked[2], nullptr);
		NewNaked[1] = PTR_TO_SBLOCK(cb);
		_DebugAttach(NEEDS_DBG_ARG NewNaked, BlockLength, RealLength);
		return (buffer) &NewNaked[2];
	}

	void Buffer::_Free(const buffer buf)
	{
		const nakedbuffer OldNaked = _Convert(buf);
		_DebugDetach(OldNaked);
		Memory::Free((void*) OldNaked);
	}

	const nakedbuffer Buffer::_Convert(const buffer buf)
	{
		PX_ASSERT("버퍼가 nullptr입니다", buf);
		const nakedbuffer Naked = (const nakedbuffer) &buf[-2];
        #ifdef PX_DEBUG
            PX_ASSERT("버퍼가 손상되었습니다", Naked[0] == ~Naked[2 + CALC_BLOCK_LEN(_SizeOf(Naked) * Naked[0])]);
        #endif
		return Naked;
	}

	sint32 Buffer::_SizeOf(const nakedbuffer naked)
	{
		PX_ASSERT("이미 삭제된 버퍼입니다", 0 <= naked[0]);
		sblock SizeOfCmd[3] = {0, NO_CHANGE_FLAG, 0};
		((_AssignorCB) naked[1])(nullptr, SizeOfCmd);
		return (sint32) SizeOfCmd[0];
	}

	chars Buffer::_NameOf(_AssignorCB cb)
	{
		sblock TypeOfCmd[3] = {0, NO_CHANGE_FLAG, 1};
		cb(nullptr, TypeOfCmd);
		return (chars) TypeOfCmd[2];
	}

	void* Buffer::_At(const buffer buf, sint32 index, _AssignorCB cb)
	{
		const nakedbuffer Naked = _Convert(buf);
		PX_ASSERT("허용범위를 초과한 인덱스입니다", 0 <= index && index < Naked[0]);
		#ifdef PX_DEBUG
			if(cb != (_AssignorCB) Naked[1])
			{
				chars Name1 = _NameOf(cb);
				chars Name2 = _NameOf((_AssignorCB) Naked[1]);
				chars Focus1 = Name1, Focus2 = Name2;
				while(*Focus1 && *(Focus1++) == *(Focus2++));
				PX_ASSERT("접근타입이 다르며 상속관계도 아닙니다", *Focus1 == '\0');
			}
		#endif
		return ((uint08*) buf) + _SizeOf(Naked) * index;
	}

	void Buffer::_Mutex(bool lock)
	{
		static MUTEX_ID MutexID = MUTEX_INIT;
		if(lock) MUTEX_LOCK(MutexID);
		else MUTEX_UNLOCK(MutexID);
	}

	void Buffer::_DebugAttach(NEEDS_DBG_PRM const nakedbuffer naked, sint32 blocklen, sint32 reallen)
	{
		#ifdef PX_DEBUG
			sblock* DebugInfo = &naked[2 + blocklen];
			DebugInfo[0] = ~naked[0]; // ~Count정보
			DebugInfo[1] = (sblock) reallen;
			DebugInfo[2] = (sblock) NEEDS_DBG_FILE;
			DebugInfo[3] = (sblock) NEEDS_DBG_LINE;
			DebugInfo[4] = (sblock) NEEDS_DBG_FUNC;
			_Mutex(true);
				DebugInfo[LINK_L] = _DebugLink()[LINK_L];
				DebugInfo[LINK_R] = (sblock) _DebugLink();
				((sblock*) _DebugLink()[LINK_L])[LINK_R] = (sblock) DebugInfo;
				_DebugLink()[LINK_L] = (sblock) DebugInfo;
			_Mutex(false);
		#endif
	}

	void Buffer::_DebugDetach(const nakedbuffer naked)
	{
		#ifdef PX_DEBUG
			const sint32 BlockLength = CALC_BLOCK_LEN(_SizeOf(naked) * naked[0]);
			sblock* DebugInfo = &naked[2 + BlockLength];
			_Mutex(true);
				((sblock*) DebugInfo[LINK_L])[LINK_R] = DebugInfo[LINK_R];
				((sblock*) DebugInfo[LINK_R])[LINK_L] = DebugInfo[LINK_L];
			_Mutex(false);
		#endif
	}

	sblock* Buffer::_DebugLink()
	{
		#ifdef PX_DEBUG
			static sblock Root[SIZE_OF_DEBUGINFO] = {~((sblock) 0), (sblock) 0,
				(sblock) __FILE__, (sblock) __LINE__, (sblock) __FUNCTION__, (sblock) Root, (sblock) Root};
			return Root;
		#else
			return nullptr;
		#endif
	}
}
