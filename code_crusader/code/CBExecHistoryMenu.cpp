/******************************************************************************
 CBExecHistoryMenu.cpp

	Maintains a fixed-length list of executable commands.

	BASE CLASS = JXStringHistoryMenu

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBExecHistoryMenu.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jcore/jAssert.h>

#include <jx_executable_small.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBExecHistoryMenu::CBExecHistoryMenu
	(
	const JSize			historyLength,
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
	JXStringHistoryMenu(historyLength, title, enclosure, hSizing, vSizing, x,y, w,h)
{
	CBExecHistoryMenuX();
}

CBExecHistoryMenu::CBExecHistoryMenu
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXStringHistoryMenu(historyLength, owner, itemIndex, enclosure)
{
	CBExecHistoryMenuX();
}

// private

void
CBExecHistoryMenu::CBExecHistoryMenuX()
{
	SetDefaultIcon(GetDisplay()->GetImageCache()->GetImage(jx_executable_small), false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBExecHistoryMenu::~CBExecHistoryMenu()
{
}
