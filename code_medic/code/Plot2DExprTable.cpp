/******************************************************************************
 Plot2DExprTable.cpp

	BASE CLASS = JXStringTable

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "Plot2DExprTable.h"
#include "globals.h"
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kMinColWidth   = 20;
const JCoordinate kExprColWidth  = 195;
const JCoordinate kRangeColWidth = 40;

/******************************************************************************
 Constructor

 ******************************************************************************/

Plot2DExprTable::Plot2DExprTable
	(
	JXMenuBar*			menuBar,
	JStringTableData*	data,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStringTable(data, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	SetSelectionBehavior(false, false);

	JXTEBase* te = GetEditMenuHandler();
	te->AppendEditMenu(menuBar);

	SetColWidth(kXExprColIndex, kExprColWidth);
	SetColWidth(kYExprColIndex, kExprColWidth);
	SetColWidth(kRangeMinColIndex, kRangeColWidth);
	SetColWidth(kRangeMaxColIndex, kRangeColWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Plot2DExprTable::~Plot2DExprTable()
{
}

/******************************************************************************
 ConfigureColHeader

 ******************************************************************************/

void
Plot2DExprTable::ConfigureColHeader
	(
	JXColHeaderWidget* colHeader
	)
{
	colHeader->SetColTitle(kXExprColIndex, JGetString("XExprColTitle::Plot2DExprTable"));
	colHeader->SetColTitle(kYExprColIndex, JGetString("YExprColTitle::Plot2DExprTable"));
	colHeader->SetColTitle(kRangeMinColIndex, JGetString("RangeMinColTitle::Plot2DExprTable"));
	colHeader->SetColTitle(kRangeMaxColIndex, JGetString("RangeMaxColTitle::Plot2DExprTable"));

	colHeader->TurnOnColResizing(kMinColWidth);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
Plot2DExprTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (ScrollForWheel(button, modifiers))
	{
		// work has been done
	}
	else if (!GetCell(pt, &cell))
	{
		s.ClearSelection();
	}
	else if (button == kJXLeftButton && clickCount == 2)
	{
		BeginEditing(cell);
	}
	else if (button == kJXLeftButton && clickCount == 1)
	{
		SelectSingleCell(cell, false);
	}
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
Plot2DExprTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJReturnKey)
	{
		JPoint cell;
		if (!IsEditing() && GetTableSelection().GetSingleSelectedCell(&cell))
		{
			BeginEditing(cell);
		}
		else
		{
			EndEditing();
		}
	}

	else if (!IsEditing() && HandleSelectionKeyPress(c, modifiers))
	{
		// work has been done
	}

	else
	{
		JXStringTable::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
Plot2DExprTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	JXInputField* input = JXStringTable::CreateXInputField(cell, x,y, w,h);
	input->SetIsRequired();
	return input;
}

/******************************************************************************
 CreateStringTableInput (virtual protected)

	Derived class can override this to instantiate a derived class of
	JXInputField.

 ******************************************************************************/

JXInputField*
Plot2DExprTable::CreateStringTableInput
	(
	const JPoint&		cell,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	if (cell.x == kRangeMinColIndex || cell.x == kRangeMaxColIndex)
	{
		return jnew JXIntegerInput(enclosure, hSizing, vSizing, x,y, w,h);
	}
	else
	{
		return jnew JXInputField(enclosure, hSizing, vSizing, x,y, w,h);
	}
}
