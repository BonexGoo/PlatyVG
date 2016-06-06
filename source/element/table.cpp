#include <platyvg.hpp>
#include "table.hpp"

namespace PlatyVGElement
{
	Table::Table()
	{
	}

	Table::Table(const Table& rhs)
	{
		operator=(rhs);
	}

	Table::~Table()
	{
	}

	Table& Table::operator=(const Table& rhs)
	{
		row = rhs.row;
		return *this;
	}
}
