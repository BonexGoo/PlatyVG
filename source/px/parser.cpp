#include "parser.hpp"
#include "../px/memory.hpp"

namespace Px
{
	Parser::Tag::Tag(const String key) : key(key)
	{
		PX_ASSERT("검색키의 길이가 0입니다", 0 < key.Length());
		Memory::Set(map, 0, sizeof(sint32) * 256);

		sint32 Focus = 0;
		while(key[++Focus])
			map[(sint32) key[Focus - 1]] = Focus;

		for(sint32 i = 0; i < 256; ++i)
			map[i] = Focus - map[i];
	}

	Parser::Tag::~Tag()
	{
	}

	Parser::Parser(const String pool) : pool(pool)
	{
		offset = 0;
	}

	Parser::~Parser()
	{
	}

	const bool Parser::SeekOf(const Tag& tag)
	{
		const sint32 KeyLen = tag.map[0];
		const sint08 KeyTail = tag.key[KeyLen - 1];
		for(sint32 i = offset + KeyLen - 1, iend = pool.Length(); i < iend; i += tag.map[pool[i] & 0xFF])
		{
			if(pool[i] != KeyTail) continue;
			if(!tag.key.Compare(pool, i + 1 - KeyLen))
			{
				offset = i + 1;
				return true;
			}
		}
		return false;
	}

	const sint32 Parser::ReadInt()
	{
		sint32 MinusFlug = 1;
		if(pool[offset] == '+') {++offset;}
		else if(pool[offset] == '-') {++offset; MinusFlug = -1;}

		sint32 Value = 0;
		sint08 OneChar = pool[offset];
		while('0' <= OneChar && OneChar <= '9')
		{
			Value = Value * 10 + OneChar - '0';
			OneChar = pool[++offset];
		}
		return Value * MinusFlug;
	}

	const sint32s Parser::ReadInts(const sint08 separator, const sint08 endmark)
	{
		sint32s Collector;
		do
		{
			Collector.AtAdding() = ReadInt();
			sint08 OneChar = pool[offset];
			while(OneChar == separator
				|| OneChar == ' ' || OneChar == '\t'
				|| OneChar == '\r' || OneChar == '\n')
				OneChar = pool[++offset];
		}
		while(pool[offset] != endmark && pool[offset] != '\0');
		return Collector;
	}

	const float Parser::ReadFloat()
	{
		float MinusFlug = 1;
		if(pool[offset] == '+') {++offset;}
		else if(pool[offset] == '-') {++offset; MinusFlug = -1;}

		sint32 Value = 0, DecimalLevel = 0;
		float Decimal = 0;
		sint08 OneChar = pool[offset];
		while('0' <= OneChar && OneChar <= '9')
		{
			if(0 < DecimalLevel)
			{
				DecimalLevel *= 10;
				Decimal += (OneChar - '0') / (float) DecimalLevel;
			}
			else Value = Value * 10 + OneChar - '0';

			OneChar = pool[++offset];
			if(DecimalLevel == 0 && OneChar == '.')
			{
				DecimalLevel = 1;
				OneChar = pool[++offset];
			}
		}
		return (Value + Decimal) * MinusFlug;
	}

	const floats Parser::ReadFloats(const sint08 separator, const sint08 endmark)
	{
		floats Collector;
		do
		{
			Collector.AtAdding() = ReadFloat();
			sint08 OneChar = pool[offset];
			while(OneChar == separator
				|| OneChar == ' ' || OneChar == '\t'
				|| OneChar == '\r' || OneChar == '\n')
				OneChar = pool[++offset];
		}
		while(pool[offset] != endmark && pool[offset] != '\0');
		return Collector;
	}

	const uint32 Parser::ReadHex32()
	{
		if(pool[offset] == '0'
			&& (pool[offset + 1] == 'x' || pool[offset + 1] == 'X'))
			offset += 2;

		uint32 Value = 0;
		bool IsNumeric = false, IsHexLower = false, IsHexUpper = false;
		sint08 OneChar = pool[offset];
		while((IsNumeric = ('0' <= OneChar && OneChar <= '9'))
			|| (IsHexLower = ('a' <= OneChar && OneChar <= 'f'))
			|| (IsHexUpper = ('A' <= OneChar && OneChar <= 'F')))
		{
			if(IsNumeric) Value = (Value << 4) + OneChar - '0';
			else if(IsHexLower) Value = (Value << 4) + 10 + OneChar - 'a';
			else if(IsHexUpper) Value = (Value << 4) + 10 + OneChar - 'A';
			OneChar = pool[++offset];
		}
		return Value;
	}
}
