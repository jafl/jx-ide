/******************************************************************************
 VarTreeDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2001-04 by John Lindal.

 *****************************************************************************/

#include "VarTreeDir.h"
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

VarTreeDir::VarTreeDir
	(
	CommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsCommandDir(supervisor)
{
	itsLink = GetLink();
	ListenTo(itsLink);

	BuildWindow();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

VarTreeDir::~VarTreeDir()
{
	if (HasPrefsManager())
	{
		GetPrefsManager()->SaveWindowSize(kVarTreeWindSizeID, GetWindow());
	}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
VarTreeDir::Activate()
{
	JXWindowDirector::Activate();
	itsWidget->ShouldUpdate(true);
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

bool
VarTreeDir::Deactivate()
{
	itsWidget->ShouldUpdate(false);
	return JXWindowDirector::Deactivate();
}

/******************************************************************************
 BuildWindow (private)

 *****************************************************************************/

#include "Generic-File.h"
#include "VarTreeDir-Actions.h"
#include "medic_variables_window.xpm"

void
VarTreeDir::BuildWindow()
{
	VarNode* root = itsLink->CreateVarNode();
	assert( root != nullptr );
	itsTree = jnew JTree(root);
	auto* treeList = jnew JNamedTreeList(itsTree);

// begin JXLayout

	auto* window = jnew JXWindow(this, 450,500, JString::empty);
	window->SetMinSize(150, 150);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Code_Medic_Variables");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 450,470);
	assert( scrollbarSet != nullptr );

	itsWidget =
		jnew VarTreeWidget(itsCommandDir, true, menuBar, itsTree, treeList, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 450,470);

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::VarTreeDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(true);
	GetPrefsManager()->GetWindowSize(kVarTreeWindSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_variables_window);
	window->SetIcon(icon);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::Generic_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&VarTreeDir::UpdateFileMenu,
		&VarTreeDir::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::VarTreeDir_Actions"));
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionsMenuStr);
	itsActionMenu->AttachHandlers(this,
		&VarTreeDir::UpdateActionMenu,
		&VarTreeDir::HandleActionMenu);
	ConfigureActionsMenu(itsActionMenu);

	GetApplication()->CreateWindowsMenu(menuBar);
	GetApplication()->CreateHelpMenu(menuBar, "VarTreeHelp");
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
VarTreeDir::UpdateWindowTitle
	(
	const JString& binaryName
	)
{
	JString title = binaryName;
	title += JGetString("WindowTitleSuffix::VarTreeDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
VarTreeDir::GetName()
	const
{
	return JGetString("WindowsMenuText::VarTreeDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
VarTreeDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetVariablesIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
VarTreeDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());
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
VarTreeDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		itsLink = GetLink();
		ListenTo(itsLink);

		(itsTree->GetRoot())->DeleteAllChildren();

		VarNode* root = itsLink->CreateVarNode();
		assert( root != nullptr );
		itsTree->SetRoot(root);
	}
	else
	{
		JXWindowDirector::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
VarTreeDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
VarTreeDir::HandleFileMenu
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
VarTreeDir::UpdateActionMenu()
{
	itsActionMenu->EnableItem(kAddVarCmd);

	if (itsWidget->HasSelection())
	{
		itsActionMenu->EnableItem(kDelVarCmd);
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
VarTreeDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kAddVarCmd)
	{
		itsWidget->NewExpression();
	}
	else if (index == kDelVarCmd)
	{
		itsWidget->RemoveSelection();
	}

	else if (index == kDisplayAsCStringCmd)
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

/******************************************************************************
 DisplayExpression

 ******************************************************************************/

void
VarTreeDir::DisplayExpression
	(
	const JString& expr
	)
{
	itsWidget->DisplayExpression(expr);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
VarTreeDir::ReadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	itsWidget->ReadSetup(input, vers);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
VarTreeDir::WriteSetup
	(
	std::ostream& output
	)
	const
{
	itsWidget->WriteSetup(output);
}
