/******************************************************************************
 FileHistoryMenu.cpp

	Menu listing recent documents.  This menu is an action menu.  It does
	not store state information, and all messages that are broadcast are
	meaningless to outsiders.

	BASE CLASS = JXFileHistoryMenu

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "FileHistoryMenu.h"
#include "ProjectDocument.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXImageCache.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

#include "jcc_project_file.xpm"

const JSize kHistoryLength = 40;

/******************************************************************************
 Constructor

 ******************************************************************************/

FileHistoryMenu::FileHistoryMenu
	(
	const DocumentManager::FileHistoryType	type,

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
	JXFileHistoryMenu(kHistoryLength, title, enclosure, hSizing, vSizing, x,y, w,h),
	itsDocType(type)
{
	FileHistoryMenuX(type);
}

FileHistoryMenu::FileHistoryMenu
	(
	const DocumentManager::FileHistoryType	type,

	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXFileHistoryMenu(kHistoryLength, owner, itemIndex, enclosure),
	itsDocType(type)
{
	FileHistoryMenuX(type);
}

// private

void
FileHistoryMenu::FileHistoryMenuX
	(
	const DocumentManager::FileHistoryType type
	)
{
	if (type == DocumentManager::kProjectFileHistory)
	{
		SetDefaultIcon(GetDisplay()->GetImageCache()->GetImage(jcc_project_file), false);
	}

	FileHistoryMenu* master =
		GetDocumentManager()->GetFileHistoryMenu(itsDocType);
	if (master != nullptr)
	{
		std::ostringstream data;
		master->WriteSetup(data);
		const std::string s = data.str();
		std::istringstream input(s);
		ReadSetup(input);
	}

	ListenTo(this);
	ListenTo(GetDocumentManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileHistoryMenu::~FileHistoryMenu()
{
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
FileHistoryMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		// need copy because menu will change
		const JString fileName = GetFile(selection->GetIndex());

		bool saveReopen = ProjectDocument::WillReopenTextFiles();
		if ((GetDisplay()->GetLatestKeyModifiers()).GetState(kJXShiftKeyIndex))
		{
			ProjectDocument::ShouldReopenTextFiles(!saveReopen);
		}

		DocumentManager* docMgr = GetDocumentManager();
		if (!(GetDisplay()->GetLatestKeyModifiers()).meta() ||
			(docMgr->CloseProjectDocuments() &&
			 docMgr->CloseTextDocuments()))
		{
			docMgr->OpenSomething(fileName);
		}

		ProjectDocument::ShouldReopenTextFiles(saveReopen);
	}

	else if (sender == GetDocumentManager() &&
			 message.Is(DocumentManager::kAddFileToHistory))
	{
		const auto* info =
			dynamic_cast<const DocumentManager::AddFileToHistory*>(&message);
		assert( info != nullptr );
		if (itsDocType == info->GetFileHistoryType())
		{
			AddFile(info->GetFullName());
		}
	}

	else
	{
		JXFileHistoryMenu::Receive(sender, message);
	}
}
