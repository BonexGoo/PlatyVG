#include "file.hpp"

#ifdef __BX_APPLICATION
	#include <BxCore.hpp>
	#define PX_OPEN(NAME, TYPE) ((void*) BxCore::File::Open(NAME, TYPE))
	#define PX_CLOSE(PTR) BxCore::File::Close((id_file) PTR)
	#define PX_GETSIZE(PTR) BxCore::File::GetSize((id_file) PTR)
	#define PX_READ(PTR, DST, SIZE) BxCore::File::Read((id_file) PTR, DST, SIZE)
	#define PX_WRITE(PTR, SRC, SIZE) BxCore::File::Write((id_file) PTR, SRC, SIZE)
	#define PX_SKIP(PTR, SIZE) BxCore::File::Skip((id_file) PTR, SIZE)
#else
    extern "C"
    {
		struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };
		typedef struct _iobuf FILE;
        extern FILE* fopen(const char*, const char*);
		extern int fclose(FILE*);
		extern long ftell(FILE*);
        extern ublock fread(void*, ublock, ublock, FILE*);
        extern ublock fwrite(const void*, ublock, ublock, FILE*);
		extern int fseek(FILE*, long, int);
    }
	#define SEEK_CUR    1
	#define SEEK_END    2
	#define SEEK_SET    0
	#define PX_OPEN(NAME, TYPE) ((void*) fopen(NAME, TYPE))
	#define PX_CLOSE(PTR) fclose((FILE*) PTR)
	#define PX_GETSIZE(PTR) fgetsize((FILE*) PTR)
	#define PX_READ(PTR, DST, SIZE) fread(DST, 1, SIZE, (FILE*) PTR)
	#define PX_WRITE(PTR, SRC, SIZE) fwrite(SRC, 1, SIZE, (FILE*) PTR)
	#define PX_SKIP(PTR, SIZE) fseek((FILE*) PTR, SIZE, SEEK_CUR)
	static long fgetsize(FILE* file)
	{
		long CurrentPos = ftell(file);
		fseek(file, 0, SEEK_END);
		long Result = ftell(file);
		fseek(file, CurrentPos, SEEK_SET);
		return Result;
	}
#endif

namespace Px
{
	const void* File::OpenForRead(chars filename)
	{
		return PX_OPEN(filename, "rb");
	}

	void* File::OpenForWrite(chars filename)
	{
		return PX_OPEN(filename, "wb");
	}

	void File::Close(const void* file)
	{
		PX_CLOSE(file);
	}

	const sint32 File::GetSize(const void* file)
	{
		return PX_GETSIZE(file);
	}

	void File::Read(const void* file, uint08* data, const sint32 size)
	{
		PX_READ(file, data, size);
	}

	void File::Write(void* file, bytes data, const sint32 size)
	{
		PX_WRITE(file, data, size);
	}

	void File::Skip(const void* file, const sint32 size)
	{
		PX_SKIP(file, size);
	}
}
