#include <platyvg.hpp>
#include "record.hpp"
#include "../core/document.hpp"

namespace PlatyVGCore
{
	Record::Record()
	{
		ActionFocus = 0;
		ActionLength = 0;
		ActionLengthMax = 100;
	}

	Record::~Record()
	{
	}

	void Record::Commit(Document* doc, buffer action)
	{
		doc->ShapeRec.AllActions.AtAdding() = action;
	}

	sint32 Record::Undo(Document* doc)
	{
		return 0; //////////////////////////////
	}

	sint32 Record::Redo(Document* doc)
	{
		return 0; //////////////////////////////
	}

	sint32 Record::GetFocus(Document* doc)
	{
		return doc->ShapeRec.ActionFocus;
	}

	sint32 Record::GetCount(Document* doc)
	{
		return doc->ShapeRec.ActionLength;
	}

	void Record::SetCountMax(Document* doc, sint32 value)
	{
		doc->ShapeRec.ActionLengthMax = value;
	}

	sint32 Record::GetCountMax(Document* doc)
	{
		return doc->ShapeRec.ActionLengthMax;
	}
}
