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
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jcore/jAssert.h>

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
	if (HasPrefsManager())
	{
		GetPrefsManager()->SaveWindowSize(kRegistersWindowSizeID, GetWindow());
	}

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

#include "Generic-File.h"
#include "medic_registers_window.xpm"

void
RegistersDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,500, JString::empty);
	window->SetMinSize(150, 150);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Code_Medic_Registers");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 450,470);
	assert( scrollbarSet != nullptr );

	itsWidget =
		jnew JXStaticText(JString::empty, false, true, true, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 450,470);

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::RegistersDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(true);
	GetPrefsManager()->GetWindowSize(kRegistersWindowSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_registers_window);
	window->SetIcon(icon);

	JString name;
	JSize size;
	GetPrefsManager()->GetDefaultFont(&name, &size);
	itsWidget->SetFont(JFontManager::GetFont(name, size));

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::Generic_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&RegistersDir::UpdateFileMenu,
		&RegistersDir::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	itsWidget->AppendEditMenu(menuBar);

	GetApplication()->CreateWindowsMenu(menuBar);
	GetApplication()->CreateHelpMenu(menuBar, "VarTreeHelp-Registers");
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
	else if (sender == GetLink() && message.Is(Link::kProgramStopped))
	{
		auto& info = dynamic_cast<const Link::ProgramStopped&>(message);
		if (!info.IsWaitingForThread())
		{
			itsNeedsUpdateFlag = true;
			Update();
		}
	}
	else if (sender == GetLink() && VarNode::ShouldUpdate(message))
	{
		itsNeedsUpdateFlag = true;
		Update();
	}

	else if (sender == GetLink() && message.Is(Link::kSymbolsLoaded))
	{
		auto& info = dynamic_cast<const Link::SymbolsLoaded&>(message);
		UpdateWindowTitle(info.GetProgramName());
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
			itsCmd->Send();
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
	itsFileMenu->SetItemEnabled(kCloseWindowCmd, !GetWindow()->IsDocked());
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
