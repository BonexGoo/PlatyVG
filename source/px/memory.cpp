#include "memory.hpp"

#ifdef __BX_APPLICATION
	#include <BxCore.hpp>
	#define PX_ALLOC(SIZE) BxCore::Util::Alloc(SIZE)
	#define PX_FREE(PTR) BxCore::Util::Free(PTR)
	#define PX_MEMSET(DST, VALUE, SIZE) BxCore::Util::MemSet(DST, VALUE, SIZE)
	#define PX_MEMCPY(DST, SRC, SIZE) BxCore::Util::MemCpy(DST, SRC, SIZE)
	#define PX_MEMCMP(DST, SRC, SIZE) BxCore::Util::MemCmp(DST, SRC, SIZE)
#else
    extern "C"
    {
        extern void* malloc(ublock);
        extern void free(void*);
        extern void* memset(void*, sint32, ublock);
        extern void* memcpy(void*, const void*, ublock);
		extern sint32 memcmp(const void*, const void*, ublock);
    }
	#define PX_ALLOC(SIZE) malloc(SIZE)
	#define PX_FREE(PTR) free((void*) PTR)
	#define PX_MEMSET(DST, VALUE, SIZE) memset(DST, VALUE, SIZE)
	#define PX_MEMCPY(DST, SRC, SIZE) memcpy(DST, SRC, SIZE)
	#define PX_MEMCMP(DST, SRC, SIZE) memcmp(DST, SRC, SIZE)
#endif

namespace Px
{
	void* Memory::Alloc(const sint32 size)
	{
		return PX_ALLOC(size);
	}

	void Memory::Free(const void* ptr)
	{
		PX_FREE(ptr);
	}

	void Memory::Set(void* ptr, const uint08 value, const sint32 size)
	{
		PX_MEMSET(ptr, value, size);
	}

	void Memory::Copy(void* dst, const void* src, const sint32 size)
	{
		PX_MEMCPY(dst, src, size);
	}

	sint32 Memory::Compare(const void* dst, const void* src, const sint32 size)
	{
		return PX_MEMCMP(dst, src, size);
	}
}
