/******************************************************************************
 RegistersDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#include "RegistersDir.h"
#include "GetRegistersCmd.h"
#include "CommandDirector.h"
#include "VarNode.h"
#include "globals.h"
#include "actionDefs.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXFontManager.h>

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

RegistersDir::RegistersDir
	(
	CommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsCommandDir(supervisor),
	itsShouldUpdateFlag(false),	// window is always initially hidden
	itsNeedsUpdateFlag(true)
{
	itsCmd = GetLink()->CreateGetRegistersCmd(this);

	BuildWindow();
	ListenTo(GetLink());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

RegistersDir::~RegistersDir()
{
	GetPrefsManager()->SaveWindowSize(kRegistersWindowSizeID, GetWindow());

	jdelete itsCmd;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
RegistersDir::Activate()
{
	JXWindowDirector::Activate();
	itsShouldUpdateFlag = true;
	Update();
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

bool
RegistersDir::Deactivate()
{
	itsShouldUpdateFlag = false;
	return JXWindowDirector::Deactivate();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "medic_registers_window.xpm"

#include <jx_file_open.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
RegistersDir::BuildWindow()
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

	window->SetTitle(JGetString("WindowTitleSuffix::RegistersDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetRegistersWindowClass());
	GetPrefsManager()->GetWindowSize(kRegistersWindowSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_registers_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	itsWidget =
		jnew JXStaticText(JString::empty, false, true, true,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert(itsWidget != nullptr);
	itsWidget->FitToEnclosure();

	JString name;
	JSize size;
	GetPrefsManager()->GetDefaultFont(&name, &size);
	itsWidget->SetFont(JFontManager::GetFont(name, size));

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "ThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsWidget->AppendEditMenu(menuBar);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "RegistersDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
RegistersDir::UpdateWindowTitle
	(
	const JString& binaryName
	)
{
	JString title = binaryName;
	title        += JGetString("WindowTitleSuffix::RegistersDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
RegistersDir::GetName()
	const
{
	return JGetString("WindowsMenuText::RegistersDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
RegistersDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetRegistersIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
RegistersDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetLink() &&
		(message.Is(Link::kProgramFinished) ||
		 message.Is(Link::kCoreCleared)     ||
		 message.Is(Link::kDetachedFromProcess)))
	{
		Update(JString::empty);
	}
	else if (sender == GetLink() &&
			 (message.Is(Link::kProgramStopped) ||
			  VarNode::ShouldUpdate(message)))
	{
		itsNeedsUpdateFlag = true;
		Update();
	}

	else if (sender == GetLink() && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());
	}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
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
RegistersDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!IsShuttingDown())
	{
		ListenTo(GetLink());

		Update(JString::empty);

		jdelete itsCmd;
		itsCmd = GetLink()->CreateGetRegistersCmd(this);
	}

	JXWindowDirector::ReceiveGoingAway(sender);
}

/******************************************************************************
 Update (private)

 ******************************************************************************/

void
RegistersDir::Update()
{
	if (itsShouldUpdateFlag && itsNeedsUpdateFlag)
	{
		itsNeedsUpdateFlag = false;

		if (itsCmd != nullptr)
		{
			itsCmd->Command::Send();
		}
	}
}

/******************************************************************************
 Update

 ******************************************************************************/

void
RegistersDir::Update
	(
	const JString& data
	)
{
	JXTEBase::DisplayState state = itsWidget->SaveDisplayState();
	itsWidget->GetText()->SetText(data);
	itsWidget->RestoreDisplayState(state);
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
RegistersDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
RegistersDir::HandleFileMenu
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
 HandleHelpMenu (private)

 ******************************************************************************/

void
RegistersDir::HandleHelpMenu
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
		JXGetHelpManager()->ShowSection("VarTreeHelp-Registers");
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
