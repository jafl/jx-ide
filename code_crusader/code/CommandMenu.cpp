/******************************************************************************
 CommandMenu.cpp

	This menu is an action menu, so the only message that is broadcast that
	is meaningful to outsiders is GetTargetInfo.  This message must be
	caught with ReceiveWithFeedback().

	BASE CLASS = JXTextMenu

	Copyright © 2001-02 by John Lindal.

 ******************************************************************************/

#include "CommandMenu.h"
#include "CommandManager.h"
#include "EditCommandsDialog.h"
#include "RunCommandDialog.h"
#include "ProjectDocument.h"
#include "SymbolDirector.h"
#include "CTreeDirector.h"
#include "DTreeDirector.h"
#include "GoTreeDirector.h"
#include "JavaTreeDirector.h"
#include "PHPTreeDirector.h"
#include "FileListDirector.h"
#include "TextDocument.h"
#include "Tree.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

#include "CommandMenu-Tasks.h"
#include "CommandMenu-AddToProject.h"
#include "CommandMenu-ManageProject.h"

// Project menu

const JSize kInitCmdCount = 4;

// JBroadcaster message types

const JUtf8Byte* CommandMenu::kGetTargetInfo = "GetTargetInfo::CommandMenu";

/******************************************************************************
 Constructor

	doc can be nullptr

 ******************************************************************************/

CommandMenu::CommandMenu
	(
	ProjectDocument*		projDoc,
	TextDocument*			textDoc,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXTextMenu(JGetString("Title::CommandMenu"), enclosure, hSizing, vSizing, x,y, w,h)
{
	CommandMenuX(projDoc, textDoc);
}

CommandMenu::CommandMenu
	(
	ProjectDocument*		projDoc,
	TextDocument*			textDoc,
	JXMenu*					owner,
	const JIndex			itemIndex,
	JXContainer*			enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	CommandMenuX(projDoc, textDoc);
}

// private

void
CommandMenu::CommandMenuX
	(
	ProjectDocument*	projDoc,
	TextDocument*		textDoc
	)
{
	assert( projDoc != nullptr || textDoc != nullptr );

	SetProjectDocument(projDoc);
	itsTextDoc        = textDoc;
	itsAddToProjMenu  = nullptr;
	itsManageProjMenu = nullptr;

	SetMenuItems(kTasksMenuStr);
	SetUpdateAction(kDisableNone);
	ConfigureTasksMenu(this);
	ListenTo(this);

	ListenTo(GetCommandManager(), std::function([this](const CommandManager::UpdateCommandMenu&)
	{
		UpdateMenu();
	}));

	if (itsTextDoc != nullptr)
	{
		itsAddToProjMenu = jnew JXTextMenu(this, kAddToProjIndex, GetEnclosure());
		assert( itsAddToProjMenu != nullptr );
		itsAddToProjMenu->SetMenuItems(kAddToProjectMenuStr);
		itsAddToProjMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsAddToProjMenu->AttachHandlers(this,
			&CommandMenu::UpdateAddToProjectMenu,
			&CommandMenu::HandleAddToProjectMenu);
		ConfigureAddToProjectMenu(itsAddToProjMenu);
	}

	itsManageProjMenu = jnew JXTextMenu(this, kManageProjIndex, GetEnclosure());
	assert( itsManageProjMenu != nullptr );
	itsManageProjMenu->SetMenuItems(kManageProjectMenuStr);
	itsManageProjMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsManageProjMenu->AttachHandlers(this,
		&CommandMenu::UpdateManageProjectMenu,
		&CommandMenu::HandleManageProjectMenu);
	ConfigureManageProjectMenu(itsManageProjMenu);

	UpdateMenu();	// build menu so shortcuts work
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CommandMenu::~CommandMenu()
{
}

/******************************************************************************
 SetProjectDocument

 ******************************************************************************/

void
CommandMenu::SetProjectDocument
	(
	ProjectDocument* projDoc
	)
{
	itsProjDoc = projDoc;
	if (itsProjDoc != nullptr)
	{
		ClearWhenGoingAway(itsProjDoc, &itsProjDoc);
	}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
CommandMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateMenu();
	}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		GetTargetInfo info;
		BuildTargetInfo(&info);
		HandleSelection(selection->GetIndex(), info);
	}

	else
	{
		JXTextMenu::Receive(sender, message);
	}
}

/******************************************************************************
 BuildTargetInfo (private)

 ******************************************************************************/

void
CommandMenu::BuildTargetInfo
	(
	GetTargetInfo* info
	)
{
	if (itsTextDoc == nullptr)
	{
		BroadcastWithFeedback(info);
	}
}

/******************************************************************************
 UpdateMenu (private)

 ******************************************************************************/

void
CommandMenu::UpdateMenu()
{
	while (GetItemCount() > kInitCmdCount)
	{
		RemoveItem(GetItemCount());
	}

	ProjectDocument* projDoc;
	const bool hasProject = GetProjectDocument(&projDoc);

	GetCommandManager()->AppendMenuItems(this, hasProject);

	if (projDoc != nullptr)
	{
		projDoc->GetCommandManager()->AppendMenuItems(this, hasProject);
	}

	// "Add to" sub-menu

	SetItemEnabled(kAddToProjIndex, CanAddToProject());

	JString itemText = JGetString("AddToProjectItemText::CommandMenu");
	if (projDoc != nullptr)
	{
		itemText += projDoc->GetName();
	}
	else
	{
		itemText += JGetString("NoProjectName::CommandMenu");
	}
	SetItemText(kAddToProjIndex, itemText);

	// "Manage" sub-menu

	SetItemEnabled(kManageProjIndex, hasProject);

	itemText = JGetString("ManageProjectItemText::CommandMenu");
	if (projDoc != nullptr)
	{
		itemText += projDoc->GetName();
	}
	else
	{
		itemText += JGetString("NoProjectName::CommandMenu");
	}
	SetItemText(kManageProjIndex, itemText);
}

/******************************************************************************
 HandleSelection (private)

 ******************************************************************************/

void
CommandMenu::HandleSelection
	(
	const JIndex			index,
	const GetTargetInfo&	info
	)
{
	ProjectDocument* projDoc = itsProjDoc;
	if (projDoc != nullptr)
	{
		GetDocumentManager()->SetActiveProjectDocument(projDoc);
	}
	else
	{
		GetDocumentManager()->GetActiveProjectDocument(&projDoc);
	}

	if (index == kRunCmd)
	{
		auto* dlog = itsTextDoc != nullptr ?
			jnew RunCommandDialog(projDoc, itsTextDoc) :
			jnew RunCommandDialog(projDoc, info.GetFileList(), info.GetLineIndexList());

		assert( dlog != nullptr );
		if (dlog->DoDialog())
		{
			dlog->Exec();
			dlog->JPrefObject::WritePrefs();
		}
	}
	else if (index == kEditCmd)
	{
		auto* dlog = jnew EditCommandsDialog(projDoc);
		if (dlog->DoDialog())
		{
			dlog->UpdateCommands();
		}
	}

	else if (index > kInitCmdCount)
	{
		JIndex i = index - kInitCmdCount;

		CommandManager* mgr = GetCommandManager();
		if (i > mgr->GetCommandCount())
		{
			assert( projDoc != nullptr );
			i  -= mgr->GetCommandCount();		// before changing mgr
			mgr = projDoc->GetCommandManager();
		}

		if (itsTextDoc != nullptr)
		{
			mgr->Exec(i, projDoc, itsTextDoc);
		}
		else
		{
			mgr->Exec(i, projDoc, info.GetFileList(), info.GetLineIndexList());
		}
	}
}

/******************************************************************************
 UpdateAddToProjectMenu (private)

 ******************************************************************************/

void
CommandMenu::UpdateAddToProjectMenu()
{
	if (CanAddToProject())
	{
		itsAddToProjMenu->EnableAll();
	}
	else
	{
		itsAddToProjMenu->DisableAll();
	}
}

/******************************************************************************
 HandleAddToProjectMenu (private)

 ******************************************************************************/

void
CommandMenu::HandleAddToProjectMenu
	(
	const JIndex index
	)
{
	if (!CanAddToProject())
	{
		return;
	}

	ProjectDocument* projDoc;
	const bool hasProject = GetProjectDocument(&projDoc);
	assert( hasProject );

	bool onDisk;
	const JString fullName = itsTextDoc->GetFullName(&onDisk);
	assert( onDisk );

	ProjectTable::PathType pathType;
	if (index == kAddToProjAbsoluteCmd)
	{
		pathType = ProjectTable::kAbsolutePath;
	}
	else if (index == kAddToProjProjRelativeCmd)
	{
		pathType = ProjectTable::kProjectRelative;
	}
	else
	{
		assert( index == kAddToProjHomeRelativeCmd );
		pathType = ProjectTable::kHomeRelative;
	}

	projDoc->AddFile(fullName, pathType);
}

/******************************************************************************
 CanAddToProject (private)

 ******************************************************************************/

bool
CommandMenu::CanAddToProject()
	const
{
	return itsProjDoc == nullptr &&
				GetDocumentManager()->HasProjectDocuments() &&
				itsTextDoc != nullptr &&
				itsTextDoc->ExistsOnDisk();
}

/******************************************************************************
 UpdateManageProjectMenu (private)

 ******************************************************************************/

void
CommandMenu::UpdateManageProjectMenu()
{
	ProjectDocument* projDoc;
	if (GetProjectDocument(&projDoc))
	{
		itsManageProjMenu->EnableAll();

		itsManageProjMenu->SetItemEnabled(kShowCTreeCmd,
			!projDoc->GetCTreeDirector()->GetTree()->IsEmpty());
		itsManageProjMenu->SetItemEnabled(kShowDTreeCmd,
			!projDoc->GetDTreeDirector()->GetTree()->IsEmpty());
		itsManageProjMenu->SetItemEnabled(kShowGoTreeCmd,
			!projDoc->GetGoTreeDirector()->GetTree()->IsEmpty());
		itsManageProjMenu->SetItemEnabled(kShowJavaTreeCmd,
			!projDoc->GetJavaTreeDirector()->GetTree()->IsEmpty());
		itsManageProjMenu->SetItemEnabled(kShowPHPTreeCmd,
			!projDoc->GetPHPTreeDirector()->GetTree()->IsEmpty());
	}
	else
	{
		itsManageProjMenu->DisableAll();
	}
}

/******************************************************************************
 HandleManageProjectMenu (private)

 ******************************************************************************/

void
CommandMenu::HandleManageProjectMenu
	(
	const JIndex index
	)
{
	ProjectDocument* projDoc;
	if (!GetProjectDocument(&projDoc))
	{
		return;
	}

	if (index == kUpdateSymbolDBCmd)
	{
		projDoc->UpdateSymbolDatabase();
	}
	else if (index == kShowSymbolBrowserCmd)
	{
		projDoc->GetSymbolDirector()->Activate();
	}
	else if (index == kShowCTreeCmd)
	{
		projDoc->GetCTreeDirector()->Activate();
	}
	else if (index == kShowDTreeCmd)
	{
		projDoc->GetDTreeDirector()->Activate();
	}
	else if (index == kShowGoTreeCmd)
	{
		projDoc->GetGoTreeDirector()->Activate();
	}
	else if (index == kShowJavaTreeCmd)
	{
		projDoc->GetJavaTreeDirector()->Activate();
	}
	else if (index == kShowPHPTreeCmd)
	{
		projDoc->GetPHPTreeDirector()->Activate();
	}
	else if (index == kShowFileListCmd)
	{
		projDoc->GetFileListDirector()->Activate();
	}
}

/******************************************************************************
 GetProjectDocument (private)

 ******************************************************************************/

bool
CommandMenu::GetProjectDocument
	(
	ProjectDocument** projDoc
	)
	const
{
	*projDoc = itsProjDoc;
	if (*projDoc == nullptr)
	{
		GetDocumentManager()->GetActiveProjectDocument(projDoc);
	}

	return *projDoc != nullptr;
}

/******************************************************************************
 UpgradeToolBarID (static)

	Returns true if we modified the id.

 ******************************************************************************/

static const JUtf8Byte* kToolbarIDMap[] =
{
	// Manage

	"CBUpdateClassTree",	"UpdateClassTree::CommandMenu",
	"CBShowSymbolBrowser",	"ShowSymbolBrowser::CommandMenu",
	"CBShowCPPClassTree",	"ShowCPPClassTree::CommandMenu",
	"CBShowDClassTree",		"ShowDClassTree::CommandMenu",
	"CBShowGoClassTree",	"ShowGoClassTree::CommandMenu",
	"CBShowJavaClassTree",	"ShowJavaClassTree::CommandMenu",
	"CBShowPHPClassTree",	"ShowPHPClassTree::CommandMenu",
	"CBShowFileList",		"ShowFileList::CommandMenu",

	// Add

	"CBAddToProjAbsolute",		"AddToProjAbsolute::CommandMenu",
	"CBAddToProjProjRelative",	"AddToProjProjRelative::CommandMenu",
	"CBAddToProjHomeRelative",	"AddToProjHomeRelative::CommandMenu",
};

const JSize kToolbarIDMapCount = sizeof(kToolbarIDMap) / sizeof(JUtf8Byte*);

bool
CommandMenu::UpgradeToolBarID
	(
	JString* s
	)
{
	if (!s->StartsWith("CB"))
	{
		return false;
	}

	for (JUnsignedOffset i=0; i<kToolbarIDMapCount; i+=2)
	{
		if (*s == kToolbarIDMap[i])
		{
			*s = kToolbarIDMap[i+1];
			return true;
		}
	}

	return false;
}
