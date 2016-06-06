#pragma once
#include <platyvg.hpp>

namespace PlatyVGElement
{
	//! \brief 연산용 테이블
	class Table
	{
	public:
		Table();
		Table(const Table& rhs);
		~Table();
		Table& operator=(const Table& rhs);

	public:
		sint32s row;
	};
	typedef Array<Table> Tables;
}
