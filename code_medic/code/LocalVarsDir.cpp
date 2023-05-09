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
#include "actionDefs.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXTextButton.h>
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
	"    Display as C string %k Meta-S       %i" kDisplayAsCStringAction
	"%l| Display as 1D array %k Meta-Shift-A %i" kDisplay1DArrayAction
	"  | Plot as 1D array                    %i" kPlot1DArrayAction
	"  | Display as 2D array                 %i" kDisplay2DArrayAction
	"%l| Watch expression                    %i" kWatchVarValueAction
	"  | Watch expression location           %i" kWatchVarLocationAction
	"%l| Examine memory                      %i" kExamineMemoryAction
	"  | Disassemble memory                  %i" kDisasmMemoryAction;

enum
{
	kDisplayAsCStringCmd = 1,
	kDisplay1DArrayCmd,
	kPlot1DArrayCmd,
	kDisplay2DArrayCmd,
	kWatchVarCmd,
	kWatchLocCmd,
	kExamineMemCmd,
	kDisassembleMemCmd
};

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

#include "medic_local_variables_window.xpm"

#include "medic_show_1d_array.xpm"
#include "medic_show_2d_plot.xpm"
#include "medic_show_2d_array.xpm"
#include "medic_show_memory.xpm"
#include <jx-af/image/jx/jx_file_open.xpm>

void
LocalVarsDir::BuildWindow()
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

	window->SetTitle(JGetString("WindowTitleSuffix::LocalVarsDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetLocalVariableWindowClass());
	GetPrefsManager()->GetWindowSize(kLocalVarWindSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_local_variables_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	VarNode* root = itsLink->CreateVarNode(false);
	assert( root != nullptr );
	itsTree = jnew JTree(root);
	assert( itsTree != nullptr );
	auto* treeList = jnew JNamedTreeList(itsTree);
	assert( treeList != nullptr );

	itsWidget =
		jnew VarTreeWidget(itsCommandDir, false, menuBar, itsTree, treeList,
							scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert(itsWidget != nullptr);
	itsWidget->FitToEnclosure();

	itsGetLocalsCmd = itsLink->CreateGetLocalVarsCmd(root);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "ThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&LocalVarsDir::UpdateFileMenu,
		&LocalVarsDir::HandleFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::global"));
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionMenuStr, "LocalVarsDir");
	itsActionMenu->AttachHandlers(this,
		&LocalVarsDir::UpdateActionMenu,
		&LocalVarsDir::HandleActionMenu);

	itsActionMenu->SetItemImage(kDisplay1DArrayCmd, medic_show_1d_array);
	itsActionMenu->SetItemImage(kPlot1DArrayCmd,    medic_show_2d_plot);
	itsActionMenu->SetItemImage(kDisplay2DArrayCmd, medic_show_2d_array);
	itsActionMenu->SetItemImage(kExamineMemCmd,     medic_show_memory);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	GetApplication()->CreateHelpMenu(menuBar, "LocalVarsDir", "VarTreeHelp-Local");
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
