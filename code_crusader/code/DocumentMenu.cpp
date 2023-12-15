/******************************************************************************
 DocumentMenu.cpp

	If Meta is down, sets selected item to active project document
	without activating it.

	BASE CLASS = JXDocumentMenu

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "DocumentMenu.h"
#include "ProjectDocument.h"
#include "SearchDocument.h"
#include "CompileDocument.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/jXMenuUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DocumentMenu::DocumentMenu
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXDocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), enclosure, hSizing, vSizing, x,y, w,h)
{
	if (JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle)
	{
		SetShortcuts(JGetString("WindowsMenuShortcut::JXGlobal"));
	}
}

DocumentMenu::DocumentMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXDocumentMenu(owner, itemIndex, enclosure)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DocumentMenu::~DocumentMenu()
{
}

/******************************************************************************
 Receive (protected)

	We absorb the ItemSelected message if the Meta key is down.

 ******************************************************************************/

void
DocumentMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kItemSelected) &&
		GetDisplay()->GetLatestKeyModifiers().GetState(JXAdjustNMShortcutModifier(kJXMetaKeyIndex)))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		if (selection->IsFromShortcut())
		{
			JXDocumentMenu::Receive(sender, message);
		}
		else
		{
			JXDocument* doc;
			DocumentManager* docMgr = GetDocumentManager();
			if (docMgr->GetDocument(selection->GetIndex(), &doc))
			{
				auto* projDoc    = dynamic_cast<ProjectDocument*>(doc);
				auto* searchDoc   = dynamic_cast<SearchDocument*>(doc);
				auto* compileDoc = dynamic_cast<CompileDocument*>(doc);
				if (projDoc != nullptr)
				{
					docMgr->SetActiveProjectDocument(projDoc);
				}
				else if (searchDoc != nullptr)
				{
					docMgr->SetActiveListDocument(searchDoc);
				}
				else if (compileDoc != nullptr)
				{
					docMgr->SetActiveListDocument(compileDoc);
				}
			}
		}
	}

	else
	{
		JXDocumentMenu::Receive(sender, message);
	}
}
