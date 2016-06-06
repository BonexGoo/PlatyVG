#include "array.hpp"

namespace Px
{
	ArrayShare::ArrayShare(const Share* share) : share(share)
	{
	}

	ArrayShare::~ArrayShare()
	{
	}

	const Share* ArrayShare::Clone() const
	{
		return share->Clone();
	}
}
