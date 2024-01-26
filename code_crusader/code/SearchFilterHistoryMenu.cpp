/******************************************************************************
 SearchFilterHistoryMenu.cpp

	Maintains a fixed-length list of paths.  When the menu needs to be
	updated, non-existent directories are removed.

	BASE CLASS = JXHistoryMenuBase

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "SearchFilterHistoryMenu.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SearchFilterHistoryMenu::SearchFilterHistoryMenu
	(
	const JSize			historyLength,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXHistoryMenuBase(historyLength, enclosure, hSizing, vSizing, x,y, w,h)
{
}

SearchFilterHistoryMenu::SearchFilterHistoryMenu
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXHistoryMenuBase(historyLength, owner, itemIndex, enclosure)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SearchFilterHistoryMenu::~SearchFilterHistoryMenu()
{
}

/******************************************************************************
 GetFilter

 ******************************************************************************/

const JString&
SearchFilterHistoryMenu::GetFilter
	(
	const Message&	message,
	bool*		invert
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const auto* selection =
		dynamic_cast<const JXMenu::ItemSelected*>(&message);
	assert( selection != nullptr );

	return GetFilter(selection->GetIndex(), invert);
}

const JString&
SearchFilterHistoryMenu::GetFilter
	(
	const JIndex	index,
	bool*		invert
	)
	const
{
	JString s;
	*invert = GetItemNMShortcut(index, &s) && s == JGetString("InvertFlag::SearchFilterHistoryMenu");

	return JXTextMenu::GetItemText(index);
}

/******************************************************************************
 AddFilter

	Prepend the given filter to the menu and remove outdated entries
	at the bottom.

 ******************************************************************************/

void
SearchFilterHistoryMenu::AddFilter
	(
	const JString&	filter,
	const bool	invert
	)
{
	if (!filter.IsEmpty())
	{
		AddItem(filter, invert ? JGetString("InvertFlag::SearchFilterHistoryMenu") : JString::empty);
	}
}
