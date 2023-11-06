/******************************************************************************
 StackDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "StackDir.h"
#include "StackWidget.h"
#include "CommandDirector.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JNamedTreeList.h>
#include <jx-af/jcore/JNamedTreeNode.h>
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

/******************************************************************************
 Constructor

 *****************************************************************************/

StackDir::StackDir
	(
	CommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsCommandDir(supervisor)
{
	BuildWindow(supervisor);
	ListenTo(GetLink());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

StackDir::~StackDir()
{
	if (HasPrefsManager())
	{
		GetPrefsManager()->SaveWindowSize(kStackWindowSizeID, GetWindow());
	}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
StackDir::Activate()
{
	JXWindowDirector::Activate();
	itsWidget->Update();
}

/******************************************************************************
 BuildWindow (private)

 *****************************************************************************/

#include "medic_stack_trace_window.xpm"

#include <jx-af/image/jx/jx_file_open.xpm>

void
StackDir::BuildWindow
	(
	CommandDirector* dir
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,500, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 450,470);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::StackDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetStackWindowClass());
	GetPrefsManager()->GetWindowSize(kStackWindowSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_stack_trace_window);
	window->SetIcon(icon);

	auto* root = jnew JNamedTreeNode(nullptr, JString::empty);
	auto* tree = jnew JTree(root);
	auto* treeList = jnew JNamedTreeList(tree);

	itsWidget =
		jnew StackWidget(dir, this, tree, treeList,
				scrollbarSet, scrollbarSet->GetScrollEnclosure(),
				JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsWidget != nullptr );
	itsWidget->FitToEnclosure();

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "StackDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&StackDir::UpdateFileMenu,
		&StackDir::HandleFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	GetApplication()->CreateHelpMenu(menuBar, "StackDir", "StackHelp");
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
StackDir::UpdateWindowTitle
	(
	const JString& binaryName
	)
{
	JString title = binaryName;
	title        += JGetString("WindowTitleSuffix::StackDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
StackDir::GetName()
	const
{
	return JGetString("WindowsMenuText::StackDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
StackDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetStackTraceIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
StackDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetLink() && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());
		itsWidget->Update();
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
StackDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!IsShuttingDown())
	{
		ListenTo(GetLink());
	}

	JXWindowDirector::ReceiveGoingAway(sender);
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
StackDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
StackDir::HandleFileMenu
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
