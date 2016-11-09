#pragma once

namespace Px {}
using namespace Px;

namespace Px
{
	typedef char				sint08;
	typedef unsigned char		uint08;
	typedef short				sint16;
	typedef unsigned short		uint16;
	typedef int					sint32;
	typedef unsigned int		uint32;
	typedef long long			sint64;
	typedef unsigned long long	uint64;
	typedef const sint08*		chars;
	typedef const uint08*		bytes;
	typedef const sint64		linkstep1;
	typedef const sint64		linkstep2;
	typedef const sint64		linkstep3;
	typedef const sint64		linkstep4;
	typedef const sint64		linkstep5;
	typedef const sint64		linkstep6;
	typedef const sint64		linkstep7;
	typedef const sint64		linkstep8;
	typedef const sint64		linkstep9;

	// About Buffer
	#if defined(__x86_64__) || defined(__amd64__) || defined(__LP64__)
		typedef long long          sblock;
		typedef unsigned long long ublock;
		typedef unsigned __int64   sizet;
		typedef __int64            ssizet;
	#else
		typedef int                sblock;
		typedef unsigned int       ublock;
		typedef unsigned int       sizet;
		typedef int                ssizet;
	#endif
	typedef sblock*                nakedbuffer;
	typedef ublock*                buffer;

    // About Platform API
    typedef sint32				api_autorun;
    #ifdef ANDROID
        typedef sint32          api_int;
        typedef uint32          api_uint;
        typedef sint32          api_flag;
        typedef sint32          api_enum;
		typedef sint64          api_long;
        typedef float           api_float;
        typedef void*           api_string;
        typedef void*           api_bytes;
		typedef void*           api_floats;
        typedef void*           api_results;
    #else
        typedef sint32          api_int;
        typedef uint32          api_uint;
        typedef sint32          api_flag;
        typedef sint32          api_enum;
		typedef sint64          api_long;
        typedef float           api_float;
        typedef chars           api_string;
        typedef bytes           api_bytes;
		typedef const float*    api_floats;
        typedef chars           api_results;
    #endif

    api_autorun CollectForPlatform(chars type, chars name, chars param);
    void ReadyForPlatform(void* env);

    api_string ReturnForPlatform(void* env, chars data);
    api_floats ReturnForPlatform(void* env, const float* data, const sint32 len);
    api_results ResultsForPlatform(void* env, char* temp, uint32 a, uint32 b);
    api_results ResultsForPlatform(void* env, char* temp, float a, float b, float c);
    api_results ResultsForPlatform(void* env, char* temp, float a, float b, float c, float d);

    class StringForPlatform
    {
        private: chars Utf8;
        private: void* Env;
        private: api_string Str;
        public: StringForPlatform(void* env, api_string str);
        public: ~StringForPlatform();
        public: operator chars() const;
    };
    class ByteArrayForPlatform
    {
        private: bytes Bytes;
        private: void* Env;
        private: api_bytes Brr;
        public: ByteArrayForPlatform(void* env, api_bytes brr);
        public: ~ByteArrayForPlatform();
        public: operator bytes() const;
    };
}

#ifdef PX_DEBUG
	#define NEEDS __FILE__,__LINE__,__FUNCTION__,
	#define NEEDS_DBG_PRM chars file,const int line,chars func,
	#define NEEDS_DBG_VAL chars File;const int Line;chars Func;
	#define NEEDS_DBG_INT File(file),Line(line),Func(func),
	#define NEEDS_DBG_ARG file,line,func,
	#define NEEDS_DBG_FILE file
	#define NEEDS_DBG_LINE line
	#define NEEDS_DBG_FUNC func
#else
	#define NEEDS
	#define NEEDS_DBG_PRM
	#define NEEDS_DBG_VAL
	#define NEEDS_DBG_INT
	#define NEEDS_DBG_ARG
	#define NEEDS_DBG_FILE ""
	#define NEEDS_DBG_LINE -1
	#define NEEDS_DBG_FUNC ""
#endif

#ifdef PX_BUILD_API
    #ifdef ANDROID
		#define PX_API_FUNC(TYPE, FUNC) \
			extern "C" TYPE Java_com_##PX_BUILD_API##_##FUNC
        #define PX_API(TYPE, FUNC, ...) \
            static api_autorun API_##FUNC = CollectForPlatform(#TYPE, #FUNC, #__VA_ARGS__); \
            PX_API_FUNC(TYPE, FUNC)(void* env, void*, __VA_ARGS__)
        #define PX_API_VOID(TYPE, FUNC) \
            static api_autorun API_##FUNC = CollectForPlatform(#TYPE, #FUNC, "void"); \
            PX_API_FUNC(TYPE, FUNC)(void* env, void*)
		#define PX_API_ENV env
    #else
		#define PX_API_FUNC(TYPE, FUNC) \
			extern "C" __declspec(dllexport) TYPE FUNC
        #define PX_API(TYPE, FUNC, ...) \
            static api_autorun API_##FUNC = CollectForPlatform(#TYPE, #FUNC, #__VA_ARGS__); \
            PX_API_FUNC(TYPE, FUNC)(__VA_ARGS__)
        #define PX_API_VOID(TYPE, FUNC) \
            static api_autorun API_##FUNC = CollectForPlatform(#TYPE, #FUNC, "void"); \
            PX_API_FUNC(TYPE, FUNC)()
		#define PX_API_ENV nullptr
    #endif
	#define PX_API_IMPL(CODE) \
        { \
			ReadyForPlatform(PX_API_ENV); \
            CODE; \
        }
    #define PX_API_IMPL_RETURN(...) \
        { \
            ReadyForPlatform(PX_API_ENV); \
            return ReturnForPlatform(PX_API_ENV, __VA_ARGS__); \
        }
    #define PX_API_IMPL_RESULTS(CODE, ...) \
        { \
			ReadyForPlatform(PX_API_ENV); \
            CODE; \
            static char temp[1024]; \
            return ResultsForPlatform(PX_API_ENV, temp, __VA_ARGS__); \
        }
    #define PX_API_STR(STR) StringForPlatform(PX_API_ENV, STR)
    #define PX_API_BRR(BRR) ByteArrayForPlatform(PX_API_ENV, BRR)
#else
	#define PX_API(TYPE, FUNC, ...)
    #define PX_API_VOID(TYPE, FUNC)
	#define PX_API_IMPL(CODE)
    #define PX_API_IMPL_RETURN(...)
    #define PX_API_IMPL_RESULTS(CODE, ...)
#endif

#if __APPLE__ == 1
	#define PTR_TO_SBLOCK(VALUE) ((sblock)(sint64) VALUE)
#else
	#define PTR_TO_SBLOCK(VALUE) ((sblock) VALUE)
#endif

#ifndef _BX_OPERATOR_NEW_
#define _BX_OPERATOR_NEW_
	inline void* operator new(sizet, sblock ptr) {return (void*) ptr;}
	inline void* operator new[](sizet, sblock ptr) {return (void*) ptr;}
	inline void operator delete(void*, sblock) {}
	inline void operator delete[](void*, sblock) {}
#endif

#ifdef __BX_APPLICATION
	#include <BxCore.hpp>
	#define PX_ASSERT(TEXT, FLAG) BxASSERT("PX<" TEXT ">", FLAG)
	#define PX_TRACE(...) BxCore::System::Trace(__VA_ARGS__)
#else
	#define PX_ASSERT(TEXT, FLAG)
	#define PX_TRACE(...)
#endif
