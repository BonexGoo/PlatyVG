#pragma once
#include <platyvg.hpp>
#include "../element/color.hpp"
#include "../element/point.hpp"

namespace PlatyVGElement
{
	//! \brief 작업내역
	class Action
	{
	public:
		Action();
		Action(const Action& rhs);
		virtual ~Action();
		Action& operator=(const Action& rhs);
	};
	typedef Union<Action> UnionAction;
	typedef Array<UnionAction> UnionActions;

	//! \brief 작업내역(생성)
	class ActionCreate : public Action
	{
	public:
		ActionCreate();
		ActionCreate(const ActionCreate& rhs);
		ActionCreate(const sint32 shapeid);
		virtual ~ActionCreate();
		ActionCreate& operator=(const ActionCreate& rhs);

	public:
		sint32 saved_shapeid;
	};

	//! \brief 작업내역(제거, 선택-제거)
	class ActionRemove : public Action
	{
	public:
		ActionRemove();
		ActionRemove(const ActionRemove& rhs);
		ActionRemove(const sint32 shapeid);
		virtual ~ActionRemove();
		ActionRemove& operator=(const ActionRemove& rhs);

	public:
		sint32 saved_shapeid;
	};

	//! \brief 작업내역(선택-변형)
	class ActionMatrix : public Action
	{
	public:
		ActionMatrix();
		ActionMatrix(const ActionMatrix& rhs);
		ActionMatrix(const sint32 shapeid, const Matrix& matrix);
		virtual ~ActionMatrix();
		ActionMatrix& operator=(const ActionMatrix& rhs);

	public:
		sint32 saved_shapeid;
		Matrix saved_matrix;
	};

	//! \brief 작업내역(선택-착색)
	class ActionColor : public Action
	{
	public:
		ActionColor();
		ActionColor(const ActionColor& rhs);
		ActionColor(const sint32 shapeid, const Color& color);
		virtual ~ActionColor();
		ActionColor& operator=(const ActionColor& rhs);

	public:
		sint32 saved_shapeid;
		Color saved_color;
	};

	//! \brief 작업내역(선택-두께변경)
	class ActionThick : public Action
	{
	public:
		ActionThick();
		ActionThick(const ActionThick& rhs);
		ActionThick(const sint32 shapeid, const float thick);
		virtual ~ActionThick();
		ActionThick& operator=(const ActionThick& rhs);

	public:
		sint32 saved_shapeid;
		float saved_thick;
	};
}
