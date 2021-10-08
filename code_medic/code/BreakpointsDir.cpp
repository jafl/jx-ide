/******************************************************************************
 BreakpointsDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "BreakpointsDir.h"
#include "BreakpointTable.h"
#include "BreakpointManager.h"
#include "CommandDirector.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXImage.h>
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
	"    Enable all breakpoints           %i" kEnableAllBreakpointsAction
	"  | Disable all breakpoints          %i" kDisableAllBreakpointsAction
	"%l| Remove all breakpoints %k Ctrl-X %i" kClearAllBreakpointsAction;

enum
{
	kEnableAllBreakpointsCmd = 1,
	kDisableAllBreakpointsCmd,
	kRemoveAllBreakpointsCmd
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

#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

/******************************************************************************
 Constructor

 *****************************************************************************/

BreakpointsDir::BreakpointsDir
	(
	CommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsCommandDir(supervisor)
{
	BuildWindow(supervisor);
	ListenTo(GetLink());
	ListenTo(GetLink()->GetBreakpointManager());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

BreakpointsDir::~BreakpointsDir()
{
	GetPrefsManager()->SaveWindowSize(kBreakpointsWindowSizeID, GetWindow());
}

/******************************************************************************
 BuildWindow (private)

 *****************************************************************************/

#include "medic_breakpoints_window.xpm"

#include <jx_file_open.xpm>

void
BreakpointsDir::BuildWindow
	(
	CommandDirector* dir
	)
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

	window->SetTitle(JGetString("WindowTitleSuffix::BreakpointsDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetBreakpointsWindowClass());
	GetPrefsManager()->GetWindowSize(kBreakpointsWindowSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_breakpoints_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	// layout table and column headers

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();

// begin tablelayout

	const JRect tablelayout_Frame    = encl->GetFrame();
	const JRect tablelayout_Aperture = encl->GetAperture();
	encl->AdjustSize(400 - tablelayout_Aperture.width(), 300 - tablelayout_Aperture.height());

	itsTable =
		jnew BreakpointTable(this, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 400,280);
	assert( itsTable != nullptr );

	itsColHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,20);
	assert( itsColHeader != nullptr );

	encl->SetSize(tablelayout_Frame.width(), tablelayout_Frame.height());

// end tablelayout

	itsTable->SetColTitles(itsColHeader);
	itsColHeader->TurnOnColResizing();

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "BreakpointsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::global"));
	itsActionMenu->SetMenuItems(kActionMenuStr, "BreakpointsDir");
	ListenTo(itsActionMenu);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "BreakpointsDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
BreakpointsDir::UpdateWindowTitle
	(
	const JString& binaryName
	)
{
	JString title = binaryName;
	title += JGetString("WindowTitleSuffix::BreakpointsDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
BreakpointsDir::GetName()
	const
{
	return JGetString("WindowsMenuText::BreakpointsDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
BreakpointsDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetBreakpointsIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
BreakpointsDir::Receive
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

	else if (sender == GetLink() && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());
	}

	else if (GetLink() != nullptr &&
			 sender == GetLink()->GetBreakpointManager() &&
			 message.Is(BreakpointManager::kBreakpointsChanged))
	{
		itsTable->Update();
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
BreakpointsDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!IsShuttingDown())
	{
		ListenTo(GetLink());
		ListenTo(GetLink()->GetBreakpointManager());
	}

	JXWindowDirector::ReceiveGoingAway(sender);
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
BreakpointsDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
BreakpointsDir::HandleFileMenu
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
BreakpointsDir::UpdateActionMenu()
{
	Link* link = GetLink();
	if (link != nullptr)
	{
		itsActionMenu->EnableItem(kEnableAllBreakpointsCmd);
		itsActionMenu->EnableItem(kDisableAllBreakpointsCmd);
		itsActionMenu->EnableItem(kRemoveAllBreakpointsCmd);
	}
}

/******************************************************************************
 HandleActionMenu

 ******************************************************************************/

void
BreakpointsDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kEnableAllBreakpointsCmd)
	{
		GetLink()->GetBreakpointManager()->EnableAll();
	}
	else if (index == kDisableAllBreakpointsCmd)
	{
		GetLink()->GetBreakpointManager()->DisableAll();
	}

	else if (index == kRemoveAllBreakpointsCmd)
	{
		GetLink()->RemoveAllBreakpoints();
	}
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
BreakpointsDir::HandleHelpMenu
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
		JXGetHelpManager()->ShowSection("BreakpointsHelp");
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
