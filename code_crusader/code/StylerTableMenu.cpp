/******************************************************************************
 StylerTableMenu.cpp

	Menu for changing font styles in a StylerTable.  This menu is an action
	menu, so all messages that are broadcast are meaningless to outsiders.

	BASE CLASS = JXStyleMenu

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "StylerTableMenu.h"
#include <jx-af/jx/JXStyleTable.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

StylerTableMenu::StylerTableMenu
	(
	JXStyleTable*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStyleMenu(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsTable = enclosure;

	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		SetItemNMShortcut(i, JString::empty);
	}

	SetUpdateAction(kDisableNone);

	Hide();
	SetToHiddenPopupMenu(true);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StylerTableMenu::~StylerTableMenu()
{
}

/******************************************************************************
 GetFontStyleForMenuUpdate (virtual protected)

 ******************************************************************************/

JFontStyle
StylerTableMenu::GetFontStyleForMenuUpdate()
	const
{
	return itsTable->GetCellStyle(itsPopupCell);
}

/******************************************************************************
 HandleMenuItem (virtual protected)

 ******************************************************************************/

void
StylerTableMenu::HandleMenuItem
	(
	const JIndex index
	)
{
	JTableSelectionIterator iter(&(itsTable->GetTableSelection()));

	const JFontStyle origStyle = GetFontStyleForMenuUpdate();
	JPoint cell;
	while (iter.Next(&cell))
	{
		JFontStyle style = itsTable->GetCellStyle(cell);
		UpdateStyle(index, origStyle, &style);
		itsTable->SetCellStyle(cell, style);
	}
}

/******************************************************************************
 UpdateStyle (protected)

 ******************************************************************************/

void
StylerTableMenu::UpdateStyle
	(
	const JIndex		index,
	const JFontStyle&	origStyle,
	JFontStyle*			style
	)
{
	if (index == kPlainStyleCmd)
	{
		*style = JFontStyle();
	}

	else if (index == kBoldStyleCmd)
	{
		style->bold = !origStyle.bold;
	}

	else if (index == kItalicStyleCmd)
	{
		style->italic = !origStyle.italic;
	}

	else if (index == kUnderlineStyleCmd && origStyle.underlineCount != 1)
	{
		style->underlineCount = 1;
	}
	else if (index == kUnderlineStyleCmd)
	{
		style->underlineCount = 0;
	}

	else if (index == kDblUnderlineStyleCmd && origStyle.underlineCount != 2)
	{
		style->underlineCount = 2;
	}
	else if (index == kDblUnderlineStyleCmd)
	{
		style->underlineCount = 0;
	}

	else if (index == kStrikeStyleCmd)
	{
		style->strike = !origStyle.strike;
	}

	else if (index >= kFirstColorCmd)
	{
		style->color = GetSelectedColor();
	}
}
