/******************************************************************************
 KeyScriptTableBase.cpp

	BASE CLASS = JXStringTable

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "KeyScriptTableBase.h"
#include "KeyScriptInput.h"
#include "EditMacroDialog.h"
#include "globals.h"
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

KeyScriptTableBase::KeyScriptTableBase
	(
	EditMacroDialog*	dialog,
	JXTextButton*		addRowButton,
	JXTextButton*		removeRowButton,
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
	JXStringTable(jnew JStringTableData,
				  scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsDialog = dialog;

	itsAddRowButton = addRowButton;
	ListenTo(itsAddRowButton);

	itsRemoveRowButton = removeRowButton;
	itsRemoveRowButton->Deactivate();
	ListenTo(itsRemoveRowButton);

	JStringTableData* data = GetStringData();
	data->AppendCols(2);	// macro, script
	FitToEnclosure();		// make sure SetColWidth() won't fail
	ListenTo(this);			// adjust script col width

	JString fontName;
	JSize fontSize;
	GetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	SetFont(fontName, fontSize);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

KeyScriptTableBase::~KeyScriptTableBase()
{
	jdelete GetStringData();
}

/******************************************************************************
 ContentsValid

	Check that the action/macro names (left column) are unique.

 ******************************************************************************/

bool
KeyScriptTableBase::ContentsValid()
	const
{
	auto* me = const_cast<KeyScriptTableBase*>(this);
	if (!me->EndEditing())
	{
		return false;
	}

	const JStringTableData* data = GetStringData();
	const JSize rowCount         = GetRowCount();
	for (JIndex i=1; i<rowCount; i++)
	{
		const JString& s1 = data->GetElement(i, kMacroColumn);
		for (JIndex j=i+1; j<=rowCount; j++)
		{
			const JString& s2 = data->GetElement(j, kMacroColumn);
			if (s1 == s2)
			{
				JTableSelection& s = me->GetTableSelection();
				s.ClearSelection();
				s.SelectRow(i);
				s.SelectRow(j);
				me->TableScrollToCell(JPoint(1,i));
				JGetUserNotification()->ReportError(
					JGetString("MustBeUnique::KeyScriptTableBase"));
				return false;
			}
		}
	}

	return true;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
KeyScriptTableBase::Activate()
{
	JXStringTable::Activate();
	if (WouldBeActive())
	{
		itsAddRowButton->Activate();
	}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
KeyScriptTableBase::Deactivate()
{
	JXStringTable::Deactivate();
	if (!WouldBeActive())
	{
		itsAddRowButton->Deactivate();
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
KeyScriptTableBase::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && GetCell(pt, &cell))
	{
		BeginEditing(cell);
	}
	else
	{
		ScrollForWheel(button, modifiers);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
KeyScriptTableBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddRowButton && message.Is(JXButton::kPushed))
	{
		AddRow();
	}
	else if (sender == itsRemoveRowButton && message.Is(JXButton::kPushed))
	{
		RemoveRow();
	}

	else
	{
		if (sender == this && message.Is(kColWidthChanged))
		{
			// do it regardless of which column changed so they can't shrink script column
			AdjustColWidths();
		}

		JXStringTable::Receive(sender, message);
	}
}

/******************************************************************************
 AddRow (private)

 ******************************************************************************/

void
KeyScriptTableBase::AddRow()
{
	if (itsDialog->ContentsValid())
	{
		GetStringData()->AppendRows(1);
		BeginEditing(JPoint(1, GetRowCount()));
	}
}

/******************************************************************************
 RemoveRow (private)

 ******************************************************************************/

void
KeyScriptTableBase::RemoveRow()
{
	JPoint editCell;
	if (GetEditedCell(&editCell))
	{
		CancelEditing();
		GetStringData()->RemoveRow(editCell.y);
	}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
KeyScriptTableBase::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXStringTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
KeyScriptTableBase::AdjustColWidths()
{
	const JSize apWidth = GetApertureWidth();

	JCoordinate lineWidth;
	JColorID color;
	GetColBorderInfo(&lineWidth, &color);
	const JSize usedWidth = GetColWidth(kMacroColumn) + lineWidth;

	if (apWidth > usedWidth)
	{
		SetColWidth(kScriptColumn, apWidth - usedWidth);
	}
	else
	{
		const JSize macroWidth = apWidth/3;
		SetColWidth(kMacroColumn, macroWidth);
		SetColWidth(kScriptColumn, apWidth - macroWidth - lineWidth);
	}
}

/******************************************************************************
 CreateStringTableInput (virtual protected)

 ******************************************************************************/

JXInputField*
KeyScriptTableBase::CreateStringTableInput
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
	itsRemoveRowButton->Activate();

	if (cell.x == kScriptColumn)
	{
		auto* obj =
			jnew KeyScriptInput(enclosure, hSizing, vSizing, x,y, w,h);
		assert( obj != nullptr );
		return obj;
	}
	else
	{
		JXInputField* input =
			JXStringTable::CreateStringTableInput(
								cell, enclosure, hSizing, vSizing, x,y, w,h);
		input->SetIsRequired();
		return input;
	}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
KeyScriptTableBase::PrepareDeleteXInputField()
{
	JXStringTable::PrepareDeleteXInputField();
	itsRemoveRowButton->Deactivate();
}
