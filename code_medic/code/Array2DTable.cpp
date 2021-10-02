/******************************************************************************
 Array2DTable.cpp

	BASE CLASS = JXStringTable

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "Array2DTable.h"
#include "Array2DDir.h"
#include "CommandDirector.h"
#include "globals.h"
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXSelectionManager.h>
#include <jx-af/jx/JXTextSelection.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

Array2DTable::Array2DTable
	(
	CommandDirector*	cmdDir,
	Array2DDir*		arrayDir,
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
	JXStringTable(data, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsCmdDir(cmdDir),
	itsArrayDir(arrayDir)
{
	SetSelectionBehavior(false, false);

	JXTEBase* te = GetEditMenuHandler();
	itsEditMenu  = te->AppendEditMenu(menuBar);
	ListenTo(itsEditMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Array2DTable::~Array2DTable()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
Array2DTable::HandleMouseDown
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
	else if (button == kJXLeftButton && modifiers.meta())
	{
		if (!s.IsSelected(cell))
		{
			SelectSingleCell(cell, false);
		}

		const JString expr = itsArrayDir->GetExpression(cell);
		itsCmdDir->DisplayExpression(expr);
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
Array2DTable::HandleKeyPress
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
Array2DTable::CreateXInputField
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

	itsOrigEditValue = input->GetText()->GetText();

	return input;
}

/******************************************************************************
 ExtractInputData

 ******************************************************************************/

bool
Array2DTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	JXInputField* input = nullptr;
	const bool ok = GetXInputField(&input);
	assert( ok );
	const JString& text = input->GetText()->GetText();

	if (!text.IsEmpty())
	{
		if (text != itsOrigEditValue)
		{
			const JString name = itsArrayDir->GetExpression(cell);
			GetLink()->SetValue(name, text);
		}
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Print header (virtual protected)

 ******************************************************************************/

JCoordinate
Array2DTable::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return p.GetLineHeight();
}

void
Array2DTable::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	p.String(pageRect.left, pageRect.top, itsArrayDir->GetExpression());

	const JString dateStr = JGetTimeStamp();
	p.String(pageRect.left, pageRect.top, dateStr,
			 pageRect.width(), JPainter::kHAlignRight);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
Array2DTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		if (HasFocus())
		{
			UpdateEditMenu();
		}
	}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
	{
		if (HasFocus())
		{
			const auto* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			HandleEditMenu(selection->GetIndex());
		}
	}

	else
	{
		JXStringTable::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
Array2DTable::UpdateEditMenu()
{
	JXTEBase* te = GetEditMenuHandler();

	JIndex index;
	if ((GetTableSelection()).HasSelection() &&
		te->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &index))
	{
		itsEditMenu->EnableItem(index);
	}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
Array2DTable::HandleEditMenu
	(
	const JIndex index
	)
{
	JTextEditor::CmdIndex cmd;
	JPoint cell;
	if (GetEditMenuHandler()->EditMenuIndexToCmd(index, &cmd) &&
		cmd == JTextEditor::kCopyCmd &&
		(GetTableSelection()).GetSingleSelectedCell(&cell))
	{
		auto* data =
			jnew JXTextSelection(GetDisplay(), GetStringData()->GetString(cell));
		assert( data != nullptr );

		GetSelectionManager()->SetData(kJXClipboardName, data);
	}
}
