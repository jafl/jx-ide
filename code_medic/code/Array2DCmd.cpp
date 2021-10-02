/******************************************************************************
 Array2DCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "Array2DCmd.h"
#include "Array2DDir.h"
#include "VarNode.h"
#include <jx-af/jx/JXStringTable.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JStyleTableData.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

Array2DCmd::Array2DCmd
	(
	Array2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	Command("", false, true),
	itsDirector(dir),
	itsTable(table),
	itsData(data)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Array2DCmd::~Array2DCmd()
{
}

/******************************************************************************
 PrepareToSend (virtual)

 ******************************************************************************/

void
Array2DCmd::PrepareToSend
	(
	const UpdateType	type,
	const JIndex		index,
	const JInteger		arrayIndex
	)
{
	itsType       = type;
	itsIndex      = index;
	itsArrayIndex = arrayIndex;
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
Array2DCmd::HandleFailure()
{
	HandleFailure(1, JString::empty);
	itsDirector->UpdateNext();
}

/******************************************************************************
 HandleFailure (protected)

 *****************************************************************************/

void
Array2DCmd::HandleFailure
	(
	const JIndex	startIndex,
	const JString&	value
	)
{
	const JIndex max =
		(itsType == kRow ? itsData->GetColCount() : itsData->GetRowCount());
	if (max == 0 || !ItsIndexValid())
	{
		itsDirector->UpdateNext();
		return;
	}

	// ugly, but much faster
	auto* styleData =
		const_cast<JStyleTableData*>(&(itsTable->GetStyleData()));
	const JFontStyle style = VarNode::GetFontStyle(false, false);

	if (startIndex == 1)
	{
		JString s(value);

		JPtrArray<JString> data(JPtrArrayT::kForgetAll, max);
		for (JIndex i=1; i<=max; i++)
		{
			data.Append(&s);
		}

		if (itsType == kRow)
		{
			itsData->SetRow(itsIndex, data);
			styleData->SetRow(itsIndex, style);
		}
		else
		{
			itsData->SetCol(itsIndex, data);
			styleData->SetCol(itsIndex, style);
		}
	}
	else
	{
		for (JIndex i=startIndex; i<=max; i++)
		{
			itsData->SetString(GetCell(i), value);
		}

		if (!value.IsEmpty())
		{
			if (itsType == kRow)
			{
				styleData->SetPartialRow(itsIndex, startIndex, max, style);
			}
			else
			{
				styleData->SetPartialCol(itsIndex, startIndex, max, style);
			}
		}
	}

	itsDirector->UpdateNext();
}

/******************************************************************************
 GetCell (protected)

 *****************************************************************************/

JPoint
Array2DCmd::GetCell
	(
	const JIndex i
	)
	const
{
	JPoint cell;
	if (itsType == kRow)
	{
		cell.Set(i, itsIndex);
	}
	else
	{
		cell.Set(itsIndex, i);
	}
	return cell;
}

/******************************************************************************
 ItsIndexValid (protected)

 *****************************************************************************/

bool
Array2DCmd::ItsIndexValid()
	const
{
	if (itsType == kRow)
	{
		return itsData->RowIndexValid(itsIndex) &&
					itsDirector->ArrayRowIndexValid(itsArrayIndex);
	}
	else
	{
		return itsData->ColIndexValid(itsIndex) &&
					itsDirector->ArrayColIndexValid(itsArrayIndex);
	}
}
