#include <platyvg.hpp>
#include "action.hpp"

namespace PlatyVGElement
{
	Action::Action()
	{
	}

	Action::Action(const Action& rhs)
	{
		operator=(rhs);
	}

	Action::~Action()
	{
	}

	Action& Action::operator=(const Action& rhs)
	{
		return *this;
	}

	//////////////////////////////////////////////////
	ActionCreate::ActionCreate()
	{
	}

	ActionCreate::ActionCreate(const ActionCreate& rhs)
	{
		operator=(rhs);
	}

	ActionCreate::ActionCreate(const sint32 shapeid)
	{
		saved_shapeid = shapeid;
	}

	ActionCreate::~ActionCreate()
	{
	}

	ActionCreate& ActionCreate::operator=(const ActionCreate& rhs)
	{
		saved_shapeid = rhs.saved_shapeid;
		return *this;
	}

	//////////////////////////////////////////////////
	ActionRemove::ActionRemove()
	{
	}

	ActionRemove::ActionRemove(const ActionRemove& rhs)
	{
		operator=(rhs);
	}

	ActionRemove::ActionRemove(const sint32 shapeid)
	{
		saved_shapeid = shapeid;
	}

	ActionRemove::~ActionRemove()
	{
	}

	ActionRemove& ActionRemove::operator=(const ActionRemove& rhs)
	{
		saved_shapeid = rhs.saved_shapeid;
		return *this;
	}

	//////////////////////////////////////////////////
	ActionMatrix::ActionMatrix()
	{
	}

	ActionMatrix::ActionMatrix(const ActionMatrix& rhs)
	{
		operator=(rhs);
	}

	ActionMatrix::ActionMatrix(const sint32 shapeid, const Matrix& matrix)
	{
		saved_shapeid = shapeid;
		saved_matrix = matrix;
	}

	ActionMatrix::~ActionMatrix()
	{
	}

	ActionMatrix& ActionMatrix::operator=(const ActionMatrix& rhs)
	{
		saved_shapeid = rhs.saved_shapeid;
		saved_matrix = rhs.saved_matrix;
		return *this;
	}

	//////////////////////////////////////////////////
	ActionColor::ActionColor()
	{
	}

	ActionColor::ActionColor(const ActionColor& rhs)
	{
		operator=(rhs);
	}

	ActionColor::ActionColor(const sint32 shapeid, const Color& color)
	{
		saved_shapeid = shapeid;
		saved_color = color;
	}

	ActionColor::~ActionColor()
	{
	}

	ActionColor& ActionColor::operator=(const ActionColor& rhs)
	{
		saved_shapeid = rhs.saved_shapeid;
		saved_color = rhs.saved_color;
		return *this;
	}

	//////////////////////////////////////////////////
	ActionThick::ActionThick()
	{
	}

	ActionThick::ActionThick(const ActionThick& rhs)
	{
		operator=(rhs);
	}

	ActionThick::ActionThick(const sint32 shapeid, const float thick)
	{
		saved_shapeid = shapeid;
		saved_thick = thick;
	}

	ActionThick::~ActionThick()
	{
	}

	ActionThick& ActionThick::operator=(const ActionThick& rhs)
	{
		saved_shapeid = rhs.saved_shapeid;
		saved_thick = rhs.saved_thick;
		return *this;
	}
}
