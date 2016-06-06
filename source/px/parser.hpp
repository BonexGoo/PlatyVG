#pragma once
#include "../px/string.hpp"

namespace Px
{
	//! \brief 파서지원
	class Parser
	{
	public:
		class Tag
		{
		public:
			Tag(const String key);
			~Tag();

		public:
			const String key;
			sint32 map[256];
		};

	public:
		Parser(const String pool);
		~Parser();

	public:
		const bool SeekOf(const Tag& tag);
		const sint32 ReadInt();
		const sint32s ReadInts(const sint08 separator, const sint08 endmark);
		const float ReadFloat();
		const floats ReadFloats(const sint08 separator, const sint08 endmark);
		const uint32 ReadHex32();

	private:
		const String pool;
		sint32 offset;
	};
}
