/******************************************************************************
 StylerTable.cpp

	BASE CLASS = JXStringTable

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "StylerTable.h"
#include "StylerTableInput.h"
#include "StylerTableMenu.h"
#include "PrefsManager.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

StylerTable::StylerTable
	(
	const JUtf8Byte**			typeNames,
	const JArray<JFontStyle>&	typeStyles,
	JXScrollbarSet*				scrollbarSet,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXStringTable(jnew JStringTableData,
				  scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	StylerTableX(kUnknownFT, false, nullptr, nullptr);

	JStringTableData* data = GetStringData();
	data->AppendCols(1);

	const JSize count = typeStyles.GetItemCount();
	data->AppendRows(count);
	for (JIndex i=1; i<=count; i++)
	{
		data->SetString(i,1, JString(typeNames[i-1], JString::kNoCopy));

		SetCellStyle(JPoint(1,i), typeStyles.GetItem(i));
	}
}

StylerTable::StylerTable
	(
	const TextFileType						fileType,
	const JArray<StylerBase::WordStyle>&	wordList,
	JXTextButton*							addRowButton,
	JXTextButton*							removeButton,
	JXScrollbarSet*							scrollbarSet,
	JXContainer*							enclosure,
	const HSizingOption						hSizing,
	const VSizingOption						vSizing,
	const JCoordinate						x,
	const JCoordinate						y,
	const JCoordinate						w,
	const JCoordinate						h
	)
	:
	JXStringTable(jnew JStringTableData,
				  scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	StylerTableX(fileType, true, addRowButton, removeButton);

	JStringTableData* data = GetStringData();
	data->AppendCols(1);

	const JSize count = wordList.GetItemCount();
	data->AppendRows(count);
	for (JIndex i=1; i<=count; i++)
	{
		const StylerBase::WordStyle info = wordList.GetItem(i);
		data->SetString(i,1, *info.key);

		SetCellStyle(JPoint(1,i), info.value);
	}
}

// private

void
StylerTable::StylerTableX
	(
	const TextFileType	fileType,
	const bool			allowEdit,
	JXTextButton*		addRowButton,
	JXTextButton*		removeButton
	)
{
	itsFileType      = fileType;
	itsAllowEditFlag = allowEdit;

	itsAddRowButton = addRowButton;
	if (itsAddRowButton != nullptr)
	{
		ListenTo(itsAddRowButton);
	}

	itsRemoveButton = removeButton;
	if (itsRemoveButton != nullptr)
	{
		ListenTo(itsRemoveButton);
	}

	SetSelectionBehavior(true, true);
	ListenTo(&(GetTableSelection()));

	PrefsManager* prefsMgr = GetPrefsManager();

	JString fontName;
	JSize fontSize;
	prefsMgr->GetDefaultFont(&fontName, &fontSize);
	SetFont(fontName, fontSize);

	const JColorID backColor = prefsMgr->GetColor(PrefsManager::kBackColorIndex);
	SetBackColor(backColor);
	SetFocusColor(backColor);
	SetSelectionColor(prefsMgr->GetColor(PrefsManager::kSelColorIndex));

	itsStyleMenu = jnew StylerTableMenu(this, kFixedLeft, kFixedTop, 0,0, 10,10);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StylerTable::~StylerTable()
{
	jdelete GetStringData();
}

/******************************************************************************
 GetData

	colorList can be nullptr.

 ******************************************************************************/

void
StylerTable::GetData
	(
	JArray<JFontStyle>* typeStyles
	)
	const
{
	const JSize count = GetRowCount();
	assert( count == typeStyles->GetItemCount() );

	for (JIndex i=1; i<=count; i++)
	{
		const JFontStyle style = GetCellStyle(JPoint(1,i));
		typeStyles->SetItem(i, style);
	}
}

void
StylerTable::GetData
	(
	JStringMap<JFontStyle>* wordStyles
	)
	const
{
	wordStyles->RemoveAll();

	const JStringTableData* data = GetStringData();

	const JSize count = GetRowCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JPoint cell(1,i);
		const JString& word = data->GetString(cell);
		if (!word.IsEmpty())
		{
			const JFontStyle style = GetCellStyle(cell);
			wordStyles->SetItem(word, style);
		}
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
StylerTable::HandleMouseDown
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
	else if ((button == kJXLeftButton && modifiers.meta()) ||
			 button == kJXRightButton)
	{
		if (!s.IsSelected(cell))
		{
			SelectSingleCell(cell, false);
		}
		DisplayFontMenu(cell, this, pt, buttonStates, modifiers);
	}
	else if (button == kJXLeftButton && clickCount == 2 && itsAllowEditFlag)
	{
		BeginEditing(cell);
	}
	else if (clickCount == 1)
	{
		BeginSelectionDrag(cell, button, modifiers);
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
StylerTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ContinueSelectionDrag(pt, modifiers);
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
StylerTable::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	FinishSelectionDrag();
}

/******************************************************************************
 DisplayFontMenu

 ******************************************************************************/

void
StylerTable::DisplayFontMenu
	(
	const JPoint&			cell,
	JXContainer*			mouseOwner,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsStyleMenu->PrepareForPopup(cell);
	itsStyleMenu->PopUp(mouseOwner, pt, buttonStates, modifiers);
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
StylerTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint topSelCell;
	JTableSelection& s          = GetTableSelection();
	const bool hadSelection = s.GetFirstSelectedCell(&topSelCell);

	if (c == kJReturnKey && !IsEditing())
	{
		if (itsAllowEditFlag &&
			hadSelection && s.GetSelectedCellCount() == 1)
		{
			BeginEditing(topSelCell);
		}
	}

	else if ((c == kJUpArrow || c == kJDownArrow) && !IsEditing())
	{
		if (!hadSelection && c == kJUpArrow && GetRowCount() > 0)
		{
			SelectSingleCell(JPoint(1, GetRowCount()));
		}
		else
		{
			HandleSelectionKeyPress(c, modifiers);
		}
	}

	else if ((c == 'a' || c == 'A') && modifiers.meta() && !modifiers.shift())
	{
		s.SelectAll();
	}

	else
	{
		JXStringTable::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
StylerTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddRowButton && message.Is(JXButton::kPushed))
	{
		AddRow();
	}
	else if (sender == itsRemoveButton && message.Is(JXButton::kPushed))
	{
		RemoveSelection();
	}
	else
	{
		if (sender == &(GetTableSelection()))
		{
			UpdateButtons();
		}

		JXStringTable::Receive(sender, message);
	}
}

/******************************************************************************
 AddRow (private)

 ******************************************************************************/

void
StylerTable::AddRow()
{
	if (EndEditing() && itsAllowEditFlag)
	{
		GetStringData()->AppendRows(1);
		BeginEditing(JPoint(1, GetRowCount()));
	}
}

/******************************************************************************
 RemoveSelection (private)

 ******************************************************************************/

void
StylerTable::RemoveSelection()
{
	JStringTableData* data = GetStringData();

	JPoint editCell;
	if (GetEditedCell(&editCell))
	{
		CancelEditing();
		data->RemoveRow(editCell.y);
	}
	else
	{
		JTableSelectionIterator iter(&(GetTableSelection()));

		JPoint cell;
		while (iter.Next(&cell))
		{
			data->RemoveRow(cell.y);
		}
	}
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
StylerTable::UpdateButtons()
{
	if (itsRemoveButton != nullptr && (GetTableSelection()).HasSelection())
	{
		itsRemoveButton->Activate();
	}
	else if (itsRemoveButton != nullptr)
	{
		itsRemoveButton->Deactivate();
	}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
StylerTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXStringTable::ApertureResized(dw,dh);
	SetColWidth(1, GetApertureWidth());
}

/******************************************************************************
 CreateStringTableInput (virtual protected)

 ******************************************************************************/

JXInputField*
StylerTable::CreateStringTableInput
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
	assert( enclosure == this );

	SelectSingleCell(cell, false);

	auto* obj =
		jnew StylerTableInput(itsFileType, this, hSizing, vSizing, x,y, w,h);
	return obj;
}
