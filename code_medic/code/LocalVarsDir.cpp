/******************************************************************************
 LocalVarsDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "LocalVarsDir.h"
#include "GetLocalVarsCmd.h"
#include "VarTreeWidget.h"
#include "VarNode.h"
#include "CommandDirector.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JNamedTreeList.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

LocalVarsDir::LocalVarsDir
	(
	CommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsCommandDir(supervisor),
	itsNeedsUpdateFlag(false)
{
	itsLink = GetLink();
	ListenTo(itsLink);

	BuildWindow();
	ListenTo(GetWindow());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LocalVarsDir::~LocalVarsDir()
{
	if (HasPrefsManager())
	{
		GetPrefsManager()->SaveWindowSize(kLocalVarWindSizeID, GetWindow());
	}

	jdelete itsGetLocalsCmd;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
LocalVarsDir::Activate()
{
	JXWindowDirector::Activate();
	Update();
}

/******************************************************************************
 BuildWindow (private)

 *****************************************************************************/

#include "Generic-File.h"
#include "LocalVarsDir-Actions.h"
#include "medic_local_variables_window.xpm"

void
LocalVarsDir::BuildWindow()
{
	VarNode* root = itsLink->CreateVarNode(false);
	assert( root != nullptr );
	itsTree = jnew JTree(root);
	auto* treeList = jnew JNamedTreeList(itsTree);

// begin JXLayout

	auto* window = jnew JXWindow(this, 450,500, JString::empty);
	window->SetMinSize(150, 150);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Code_Medic_Variables_Local");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 450,470);
	assert( scrollbarSet != nullptr );

	itsWidget =
		jnew VarTreeWidget(itsCommandDir, false, menuBar, itsTree, treeList, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 450,470);

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::LocalVarsDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(true);
	GetPrefsManager()->GetWindowSize(kLocalVarWindSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_local_variables_window);
	window->SetIcon(icon);

	itsGetLocalsCmd = itsLink->CreateGetLocalVarsCmd(root);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::Generic_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&LocalVarsDir::UpdateFileMenu,
		&LocalVarsDir::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::LocalVarsDir_Actions"));
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionsMenuStr);
	itsActionMenu->AttachHandlers(this,
		&LocalVarsDir::UpdateActionMenu,
		&LocalVarsDir::HandleActionMenu);
	ConfigureActionsMenu(itsActionMenu);

	GetApplication()->CreateWindowsMenu(menuBar);
	GetApplication()->CreateHelpMenu(menuBar, "VarTreeHelp-Local");
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
LocalVarsDir::UpdateWindowTitle
	(
	const JString& binaryName
	)
{
	JString title = binaryName;
	title += JGetString("WindowTitleSuffix::LocalVarsDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
LocalVarsDir::GetName()
	const
{
	return JGetString("WindowsMenuText::LocalVarsDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
LocalVarsDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetLocalVarsIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
LocalVarsDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink &&
		(message.Is(Link::kValueChanged)   ||
		 message.Is(Link::kThreadChanged)  ||
		 message.Is(Link::kFrameChanged)   ||
		 message.Is(Link::kCoreLoaded)     ||
		 message.Is(Link::kAttachedToProcess)))
	{
		Rebuild();
	}
	else if (sender == itsLink && message.Is(Link::kProgramStopped))
	{
		const auto& info =
			dynamic_cast<const Link::ProgramStopped&>(message);

		const Location* loc;
		if (info.GetLocation(&loc) && !(loc->GetFileName()).IsEmpty())
		{
			Rebuild();
		}
	}
	else if (sender == itsLink &&
			 (message.Is(Link::kProgramFinished) ||
			  message.Is(Link::kCoreCleared)     ||
			  message.Is(Link::kDetachedFromProcess)))
	{
		// can't listen for Link::kProgramRunning because this happens
		// every time the user executes another line of code

		FlushOldData();
	}

	else if (sender == itsLink && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());
	}

	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
	{
		Update();
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
LocalVarsDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		itsLink = GetLink();
		ListenTo(itsLink);

		(itsTree->GetRoot())->DeleteAllChildren();

		VarNode* root = itsLink->CreateVarNode(false);
		assert( root != nullptr );
		itsTree->SetRoot(root);

		jdelete itsGetLocalsCmd;
		itsGetLocalsCmd = itsLink->CreateGetLocalVarsCmd(root);

		itsNeedsUpdateFlag = false;
	}
	else
	{
		JXWindowDirector::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 Update (private)

 ******************************************************************************/

void
LocalVarsDir::Update()
{
	if ((itsLink->HasCore() || itsLink->ProgramIsStopped()) &&
		itsNeedsUpdateFlag)
	{
		Rebuild();
	}
}

/******************************************************************************
 Rebuild (private)

 ******************************************************************************/

void
LocalVarsDir::Rebuild()
{
	if (IsActive() && !GetWindow()->IsIconified())
	{
		itsNeedsUpdateFlag = false;	// can't call FlushOldData() since must *update* tree
		itsGetLocalsCmd->Send();

		auto* root = dynamic_cast<VarNode*>(itsTree->GetRoot());
		assert( root != nullptr );
		root->SetValid(false);
	}
	else
	{
		itsNeedsUpdateFlag = true;
	}
}

/******************************************************************************
 FlushOldData (private)

 ******************************************************************************/

void
LocalVarsDir::FlushOldData()
{
	(itsTree->GetRoot())->DeleteAllChildren();
	itsNeedsUpdateFlag = false;
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
LocalVarsDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
LocalVarsDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
	{
		itsCommandDir->OpenSourceFiles();
	}

	else if (index == kCloseWindowCmd)
	{
		Deactivate();
	}
	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 UpdateActionMenu

 ******************************************************************************/

void
LocalVarsDir::UpdateActionMenu()
{
	if (itsWidget->HasSelection())
	{
		itsActionMenu->EnableItem(kDisplayAsCStringCmd);
		itsActionMenu->EnableItem(kDisplay1DArrayCmd);
		itsActionMenu->EnableItem(kPlot1DArrayCmd);
		itsActionMenu->EnableItem(kDisplay2DArrayCmd);
		itsActionMenu->EnableItem(kWatchVarCmd);
		itsActionMenu->EnableItem(kWatchLocCmd);
	}

	if (itsLink->GetFeature(Link::kExamineMemory))
	{
		itsActionMenu->EnableItem(kExamineMemCmd);
	}
	if (itsLink->GetFeature(Link::kDisassembleMemory))
	{
		itsActionMenu->EnableItem(kDisassembleMemCmd);
	}
}

/******************************************************************************
 HandleActionMenu

 ******************************************************************************/

void
LocalVarsDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kDisplayAsCStringCmd)
	{
		itsWidget->DisplayAsCString();
	}

	else if (index == kDisplay1DArrayCmd)
	{
		itsWidget->Display1DArray();
	}
	else if (index == kPlot1DArrayCmd)
	{
		itsWidget->Plot1DArray();
	}
	else if (index == kDisplay2DArrayCmd)
	{
		itsWidget->Display2DArray();
	}

	else if (index == kWatchVarCmd)
	{
		itsWidget->WatchExpression();
	}
	else if (index == kWatchLocCmd)
	{
		itsWidget->WatchLocation();
	}

	else if (index == kExamineMemCmd)
	{
		itsWidget->ExamineMemory(MemoryDir::kHexByte);
	}
	else if (index == kDisassembleMemCmd)
	{
		itsWidget->ExamineMemory(MemoryDir::kAsm);
	}
}
