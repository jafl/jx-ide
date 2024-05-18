/******************************************************************************
 SearchPathHistoryMenu.cpp

	Maintains a fixed-length list of paths.  When the menu needs to be
	updated, non-existent directories are removed.

	BASE CLASS = JXHistoryMenuBase

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "SearchPathHistoryMenu.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

#include <jx-af/image/jx/jx_folder_small.xpm>

static const JString kOldRecurseFlag("(recurse)");
static const JString kRecurseFlag("(include subdirs)");

/******************************************************************************
 Constructor

 ******************************************************************************/

SearchPathHistoryMenu::SearchPathHistoryMenu
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
	SearchPathHistoryMenuX();
}

SearchPathHistoryMenu::SearchPathHistoryMenu
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXHistoryMenuBase(historyLength, owner, itemIndex, enclosure)
{
	SearchPathHistoryMenuX();
}

// private

void
SearchPathHistoryMenu::SearchPathHistoryMenuX()
{
	SetDefaultIcon(GetDisplay()->GetImageCache()->GetImage(jx_folder_small), false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SearchPathHistoryMenu::~SearchPathHistoryMenu()
{
}

/******************************************************************************
 GetPath

 ******************************************************************************/

const JString&
SearchPathHistoryMenu::GetPath
	(
	const Message&	message,
	bool*		recurse
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
	return GetPath(selection.GetIndex(), recurse);
}

const JString&
SearchPathHistoryMenu::GetPath
	(
	const JIndex	index,
	bool*		recurse
	)
	const
{
	JString s;
	*recurse = GetItemNMShortcut(index, &s) &&
					(s == kRecurseFlag || s == kOldRecurseFlag);

	return JXTextMenu::GetItemText(index);
}

/******************************************************************************
 AddPath

	Prepend the given path to the menu and remove outdated entries
	at the bottom.

 ******************************************************************************/

void
SearchPathHistoryMenu::AddPath
	(
	const JString&	fullName,
	const bool	recurse
	)
{
	if (!fullName.IsEmpty())
	{
		const JString path = JConvertToHomeDirShortcut(fullName);

		AddItem(path, recurse ? kRecurseFlag : JString::empty);
	}
}

/******************************************************************************
 UpdateMenu (virtual protected)

 ******************************************************************************/

void
SearchPathHistoryMenu::UpdateMenu()
{
	RemoveInvalidPaths();
	JXHistoryMenuBase::UpdateMenu();
}

/******************************************************************************
 RemoveInvalidPaths (private)

 ******************************************************************************/

void
SearchPathHistoryMenu::RemoveInvalidPaths()
{
	JString fullName;

	const JSize count       = GetItemCount();
	const JIndex firstIndex = GetFirstIndex();
	for (JIndex i=count; i>=firstIndex; i--)
	{
		const JString& dirName = JXTextMenu::GetItemText(i);
		if (JIsAbsolutePath(dirName) &&
			!JConvertToAbsolutePath(dirName, JString::empty, &fullName))
		{
			RemoveItem(i);
		}
	}
}
