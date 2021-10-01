/******************************************************************************
 DiffStyleMenu.cpp

	Menu for changing font styles for displaying file differences.  This menu
	is an action menu, so all messages that are broadcast are meaningless to
	outsiders.

	BASE CLASS = JXStyleMenu

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "DiffStyleMenu.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DiffStyleMenu::DiffStyleMenu
	(
	const JString&		title,
	JXContainer*		enclosure,
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
	SetTitle(title, nullptr, false);
	SetUpdateAction(kDisableNone);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DiffStyleMenu::~DiffStyleMenu()
{
}

/******************************************************************************
 GetFontStyleForMenuUpdate (virtual protected)

 ******************************************************************************/

JFontStyle
DiffStyleMenu::GetFontStyleForMenuUpdate()
	const
{
	return itsStyle;
}

/******************************************************************************
 HandleMenuItem (virtual protected)

 ******************************************************************************/

void
DiffStyleMenu::HandleMenuItem
	(
	const JIndex index
	)
{
	UpdateStyle(index, &itsStyle);
}

/******************************************************************************
 ReadStyle

 ******************************************************************************/

void
DiffStyleMenu::ReadStyle
	(
	std::istream& input
	)
{
	input >> JBoolFromString(itsStyle.bold)
		  >> JBoolFromString(itsStyle.italic)
		  >> JBoolFromString(itsStyle.strike);
	input >> itsStyle.underlineCount;

	JRGB color;
	input >> color;
	itsStyle.color = JColorManager::GetColorID(color);
}

/******************************************************************************
 WriteStyle

 ******************************************************************************/

void
DiffStyleMenu::WriteStyle
	(
	std::ostream& output
	)
{
	output << ' ' << JBoolToString(itsStyle.bold)
				  << JBoolToString(itsStyle.italic)
				  << JBoolToString(itsStyle.strike);
	output << ' ' << itsStyle.underlineCount;
	output << ' ' << JColorManager::GetRGB(itsStyle.color);
}
