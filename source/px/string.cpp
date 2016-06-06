#include "string.hpp"
#include "../px/math.hpp"
#include "../px/memory.hpp"

#ifdef __BX_APPLICATION
	#include <BxUtil.hpp>
	#define PX_STRSAME(A, B) (BxUtilGlobal::StrCmp(A, B) == same)
#else
    extern "C"
    {
        extern int strcmp(const char*, const char*);
    }
	#define PX_STRSAME(A, B) (strcmp(A, B) == 0)
#endif

namespace Px
{
	String::String()
	{
		words.AtAdding() = '\0';
	}

	String::String(const String& rhs)
	{
		operator=(rhs);
	}

	String::String(chars rhs)
	{
		operator=(rhs);
	}

	String::String(chars rhs, sint32 length)
	{
		words.Clear();
		for(sint32 i = 0; i < length; ++i)
			words.AtAdding() = rhs[i];
		words.AtAdding() = '\0';
	}

	String::~String()
	{
	}

	String& String::operator=(const String& rhs)
	{
		words = rhs.words;
		return *this;
	}

	String& String::operator=(chars rhs)
	{
		words.Clear();
		do {words.AtAdding() = *rhs;}
		while(*(rhs++));
		return *this;
	}

	bool String::operator==(const String& rhs) const
	{
		return PX_STRSAME(&words[0], &rhs.words[0]);
	}

	bool String::operator==(chars rhs) const
	{
		return PX_STRSAME(&words[0], rhs);
	}

	bool String::operator!=(const String& rhs) const
	{
		return !operator==(rhs);
	}

	bool String::operator!=(chars rhs) const
	{
		return !operator==(rhs);
	}

	String::operator chars() const
	{
		return &words[0];
	}

	void String::Empty()
	{
		words.Clear();
	}

	const sint08 String::operator[](sint32 index) const
	{
		return words[index];
	}

	sint32 String::Length() const
	{
		return words.Count() - 1;
	}

	sint32 String::Compare(const String& other, sint32 offset) const
	{
		PX_ASSERT("other의 범위를 초과한 접근입니다", 0 <= offset && offset + Length() <= other.Length());
		return Memory::Compare(&words[0], &other.words[offset],
			Math::Min(words.Count(), other.words.Count()));
	}
}
