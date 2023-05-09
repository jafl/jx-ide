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
#include "actionDefs.h"
#include <jx-af/jcore/jAssert.h>

// Project menu

static const JUtf8Byte* kMenuStr =
	"    Perform one-off task..."
	"  | Customize this menu..."
	"%l| Add to project: none"
	"  | Manage project: none"
	"%l";

enum
{
	kRunCmd = 1,
	kEditCmd,
	kAddToProjIndex,
	kManageProjIndex,	// = kInitCmdCount

	kInitCmdCount = kManageProjIndex
};

// Add to Project menu

static const JUtf8Byte* kAddToProjMenuStr =
	"  Absolute path                              %i" kAddToProjAbsoluteAction
	"| Relative to project file   %k Meta-Shift-A %i" kAddToProjProjRelativeAction
	"| Relative to home directory                 %i" kAddToProjHomeRelativeAction;

enum
{
	kAddToProjAbsoluteCmd = 1,
	kAddToProjProjRelativeCmd,
	kAddToProjHomeRelativeCmd
};

// Manage Project menu

#include "jcc_show_symbol_list.xpm"
#include "jcc_show_c_tree.xpm"
#include "jcc_show_d_tree.xpm"
#include "jcc_show_go_tree.xpm"
#include "jcc_show_java_tree.xpm"
#include "jcc_show_php_tree.xpm"
#include "jcc_show_file_list.xpm"

static const JUtf8Byte* kManageProjMenuStr =
	"    Update symbol database         %k Meta-U       %i" kUpdateClassTreeAction
	"%l| Show symbol browser            %k Ctrl-F12     %i" kShowSymbolBrowserAction
	"  | Show C++ class tree                            %i" kShowCPPClassTreeAction
	"  | Show D class tree                              %i" kShowDClassTreeAction
	"  | Show Go struct/interface tree                  %i" kShowGoClassTreeAction
	"  | Show Java class tree                           %i" kShowJavaClassTreeAction
	"  | Show PHP class tree                            %i" kShowPHPClassTreeAction
	"  | Show file list                 %k Meta-Shift-F %i" kShowFileListAction;

enum
{
	kUpdateSymbolDBCmd = 1,
	kShowSymbolBrowserCmd,
	kShowCTreeCmd,
	kShowDTreeCmd,
	kShowGoTreeCmd,
	kShowJavaTreeCmd,
	kShowPHPTreeCmd,
	kShowFileListCmd
};

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

	SetMenuItems(kMenuStr);
	SetUpdateAction(kDisableNone);
	ListenTo(this);

	ListenTo(GetCommandManager(), std::function([this](const CommandManager::UpdateCommandMenu&)
	{
		UpdateMenu();
	}));

	if (itsTextDoc != nullptr)
	{
		itsAddToProjMenu = jnew JXTextMenu(this, kAddToProjIndex, GetEnclosure());
		assert( itsAddToProjMenu != nullptr );
		itsAddToProjMenu->SetMenuItems(kAddToProjMenuStr, "CommandMenu");
		itsAddToProjMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsAddToProjMenu->AttachHandlers(this,
			&CommandMenu::UpdateAddToProjectMenu,
			&CommandMenu::HandleAddToProjectMenu);
	}

	itsManageProjMenu = jnew JXTextMenu(this, kManageProjIndex, GetEnclosure());
	assert( itsManageProjMenu != nullptr );
	itsManageProjMenu->SetMenuItems(kManageProjMenuStr, "CommandMenu");
	itsManageProjMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsManageProjMenu->AttachHandlers(this,
		&CommandMenu::UpdateManageProjectMenu,
		&CommandMenu::HandleManageProjectMenu);

	itsManageProjMenu->SetItemImage(kShowSymbolBrowserCmd, jcc_show_symbol_list);
	itsManageProjMenu->SetItemImage(kShowCTreeCmd,         jcc_show_c_tree);
	itsManageProjMenu->SetItemImage(kShowDTreeCmd,         jcc_show_d_tree);
	itsManageProjMenu->SetItemImage(kShowGoTreeCmd,        jcc_show_go_tree);
	itsManageProjMenu->SetItemImage(kShowJavaTreeCmd,      jcc_show_java_tree);
	itsManageProjMenu->SetItemImage(kShowPHPTreeCmd,       jcc_show_php_tree);
	itsManageProjMenu->SetItemImage(kShowFileListCmd,      jcc_show_file_list);

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
		assert( dlog != nullptr );
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
