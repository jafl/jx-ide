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
#include "actionDefs.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXImage.h>

#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JNamedTreeList.h>
#include <jx-af/jcore/jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Open source file... %k Meta-O %i" kOpenSourceFileAction
	"%l| Close               %k Meta-W %i" kJXCloseWindowAction
	"  | Quit                %k Meta-Q %i" kJXQuitAction;

enum
{
	kOpenCmd = 1,
	kCloseWindowCmd,
	kQuitCmd
};

// Actions menu

static const JUtf8Byte* kActionMenuStr =
	"    New expression      %k Meta-N       %i" kNewExpressionAction
	"  | Remove expression   %k Backspace.   %i" kRemoveExpressionAction
	"%l| Display as C string %k Meta-S       %i" kDisplayAsCStringAction
	"%l| Display as 1D array %k Meta-Shift-A %i" kDisplay1DArrayAction
	"  | Plot as 1D array                    %i" kPlot1DArrayAction
	"  | Display as 2D array                 %i" kDisplay2DArrayAction
	"%l| Watch expression                    %i" kWatchVarValueAction
	"  | Watch expression location           %i" kWatchVarLocationAction
	"%l| Examine memory                      %i" kExamineMemoryAction
	"  | Disassemble memory                  %i" kDisasmMemoryAction;

enum
{
	kAddVarCmd = 1,
	kDelVarCmd,
	kDisplayAsCStringCmd,
	kDisplay1DArrayCmd,
	kPlot1DArrayCmd,
	kDisplay2DArrayCmd,
	kWatchVarCmd,
	kWatchLocCmd,
	kExamineMemCmd,
	kDisassembleMemCmd
};

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"    About"
	"%l| Table of Contents"
	"  | Overview"
	"  | This window %k F1"
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

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
	GetPrefsManager()->SaveWindowSize(kVarTreeWindSizeID, GetWindow());
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

#include "medic_variables_window.xpm"

#include "medic_show_1d_array.xpm"
#include "medic_show_2d_plot.xpm"
#include "medic_show_2d_array.xpm"
#include "medic_show_memory.xpm"
#include <jx_file_open.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
VarTreeDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,500, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 450,470);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::VarTreeDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetVariableWindowClass());
	GetPrefsManager()->GetWindowSize(kVarTreeWindSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_variables_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	VarNode* root = itsLink->CreateVarNode();
	assert( root != nullptr );
	itsTree = jnew JTree(root);
	assert( itsTree != nullptr );
	auto* treeList = jnew JNamedTreeList(itsTree);
	assert( treeList != nullptr );

	itsWidget =
		jnew VarTreeWidget(itsCommandDir, true, menuBar, itsTree, treeList,
							scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert(itsWidget != nullptr);
	itsWidget->FitToEnclosure();

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "ThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::global"));
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionMenuStr, "VarTreeDir");
	ListenTo(itsActionMenu);

	itsActionMenu->SetItemImage(kDisplay1DArrayCmd, medic_show_1d_array);
	itsActionMenu->SetItemImage(kPlot1DArrayCmd,    medic_show_2d_plot);
	itsActionMenu->SetItemImage(kDisplay2DArrayCmd, medic_show_2d_array);
	itsActionMenu->SetItemImage(kExamineMemCmd,     medic_show_memory);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "VarTreeDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);
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
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateFileMenu();
	}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
	{
		 const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
	}

	else if (sender == itsActionMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateActionMenu();
	}
	else if (sender == itsActionMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionMenu(selection->GetIndex());
	}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
	}

	else if (sender == itsLink && message.Is(Link::kSymbolsLoaded))
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
	itsFileMenu->SetItemEnable(kCloseWindowCmd, !GetWindow()->IsDocked());
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
 HandleHelpMenu

 ******************************************************************************/

void
VarTreeDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
	{
		(GetApplication())->DisplayAbout();
	}
	else if (index == kTOCCmd)
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (index == kOverviewCmd)
	{
		JXGetHelpManager()->ShowSection("OverviewHelp");
	}
	else if (index == kThisWindowCmd)
	{
		JXGetHelpManager()->ShowSection("VarTreeHelp");
	}
	else if (index == kChangesCmd)
	{
		JXGetHelpManager()->ShowChangeLog();
	}
	else if (index == kCreditsCmd)
	{
		JXGetHelpManager()->ShowCredits();
	}
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
