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
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/jAssert.h>

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
	BuildWindow();
	ListenTo(GetLink());
	ListenTo(GetLink()->GetBreakpointManager());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

BreakpointsDir::~BreakpointsDir()
{
	if (HasPrefsManager())
	{
		GetPrefsManager()->SaveWindowSize(kBreakpointsWindowSizeID, GetWindow());
	}
}

/******************************************************************************
 BuildWindow (private)

 *****************************************************************************/

#include "Generic-File.h"
#include "BreakpointsDir-Actions.h"
#include "medic_breakpoints_window.xpm"

void
BreakpointsDir::BuildWindow()
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

	window->SetTitle(JGetString("WindowTitleSuffix::BreakpointsDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetBreakpointsWindowClass());
	GetPrefsManager()->GetWindowSize(kBreakpointsWindowSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_breakpoints_window);
	window->SetIcon(icon);

	// layout table and column headers

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();

// begin tablelayout

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

	encl->SetSize(tablelayout_Aperture.width(), tablelayout_Aperture.height());

// end tablelayout

	itsTable->SetColTitles(itsColHeader);
	itsColHeader->TurnOnColResizing();

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::Generic_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&BreakpointsDir::UpdateFileMenu,
		&BreakpointsDir::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::BreakpointsDir_Actions"));
	itsActionMenu->SetMenuItems(kActionsMenuStr);
	itsActionMenu->AttachHandlers(this,
		&BreakpointsDir::UpdateActionMenu,
		&BreakpointsDir::HandleActionMenu);
	ConfigureActionsMenu(itsActionMenu);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	GetApplication()->CreateHelpMenu(menuBar, "BreakpointsHelp");
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
	if (sender == GetLink() && message.Is(Link::kSymbolsLoaded))
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
	itsFileMenu->SetItemEnabled(kCloseWindowCmd, !GetWindow()->IsDocked());
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
