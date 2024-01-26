/******************************************************************************
 App.cpp

	BASE CLASS = JXApplication

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "App.h"
#include "MDIServer.h"
#include "AboutDialog.h"
#include "QuitTask.h"
#include "globals.h"
#include "stringData.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXDockManager.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/JSimpleProcess.h>
#include <jx-af/jcore/JSubstitute.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jWebUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// Application signature (MDI, prefs)

static const JUtf8Byte* kAppSignature = "medic";

/******************************************************************************
 Constructor

 ******************************************************************************/

App::App
	(
	int*		argc,
	char*		argv[],
	bool*		displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, "Code_Medic", kDefaultStringData)
{
	*displayAbout = CreateGlobals(this);
	if (!*displayAbout)
	{
		*prevVersStr = GetPrefsManager()->GetMedicVersionStr();
		if (*prevVersStr == GetVersionNumberStr())
		{
			prevVersStr->Clear();
		}
		else
		{
			*displayAbout = true;
		}
	}
	else
	{
		prevVersStr->Clear();
	}

	auto* mdi = jnew MDIServer;		// deleted by jXGlobals
	assert( mdi != nullptr );

	auto* task = jnew QuitTask();
	task->Start();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

App::~App()
{
	DeleteGlobals();
}

/******************************************************************************
 EditFile

	Tell text editor to display the specified file.
	If lineIndex > 0, displays the specified line of the file.

 ******************************************************************************/

void
App::EditFile
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
	const
{
	JString editFileCmd, editFileLineCmd;
	GetPrefsManager()->GetEditFileCmds(&editFileCmd, &editFileLineCmd);

	JString cmd;
	if (lineIndex > 0)
	{
		cmd = editFileLineCmd;
	}
	else
	{
		cmd = editFileCmd;
	}

	const JString name = JPrepArgForExec(fileName);
	const JString lineIndexStr(lineIndex, 0);

	JSubstitute sub;
	sub.IgnoreUnrecognized();
	sub.DefineVariable("f", name);
	sub.DefineVariable("l", lineIndexStr);
	sub.Substitute(&cmd);

	JSimpleProcess::Create(cmd, true);
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
App::DisplayAbout
	(
	const bool		showLicense,
	const JString&	prevVersStr
	)
{
	StartFiber([showLicense, prevVersStr]()
	{
		if (!showLicense || JGetUserNotification()->AcceptLicense())
		{
			auto* dlog = jnew AboutDialog(prevVersStr);
			dlog->DoDialog();

			if (showLicense && prevVersStr.IsEmpty() &&
				JGetUserNotification()->AskUserYes(
					JGetString("StartupTips::JXAskInitDockAll")))
			{
				JXGetDockManager()->DockAll();
			}

			JCheckForNewerVersion(GetPrefsManager(), kVersionCheckID);
		}
		else
		{
			ForgetPrefsManager();
			JXGetApplication()->Quit();
		}
	});
}

/******************************************************************************
 CreateWindowsMenu

 ******************************************************************************/

JXTextMenu*
App::CreateWindowsMenu
	(
	JXMenuBar* menuBar
	)
{
	auto* menu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	menuBar->AppendMenu(menu);
	if (JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle)
	{
		menu->SetShortcuts(JGetString("WindowsMenuShortcut::JXGlobal"));
	}

	return menu;
}

/******************************************************************************
 CreateHelpMenu

 ******************************************************************************/

#include "App-Help.h"

JXTextMenu*
App::CreateHelpMenu
	(
	JXMenuBar*			menuBar,
	const JUtf8Byte*	sectionName
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("MenuTitle::App_Help"));
	menu->SetMenuItems(kHelpMenuStr);
	menu->SetUpdateAction(JXMenu::kDisableNone);
	ConfigureHelpMenu(menu);

	ListenTo(menu, std::function([this, sectionName](const JXMenu::ItemSelected& msg)
	{
		HandleHelpMenu(sectionName, msg.GetIndex());
	}));

	return menu;
}

/******************************************************************************
 AppendHelpMenuToToolBar

 ******************************************************************************/

void
App::AppendHelpMenuToToolBar
	(
	JXToolBar*	toolBar,
	JXTextMenu* menu
	)
{
	toolBar->NewGroup();
	toolBar->AppendButton(menu, kHelpTOCCmd);
	toolBar->AppendButton(menu, kHelpWindowCmd);
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
App::HandleHelpMenu
	(
	const JUtf8Byte*	windowSectionName,
	const JIndex		index
	)
{
	if (index == kHelpAboutCmd)
	{
		DisplayAbout();
	}

	else if (index == kHelpTOCCmd)
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (index == kHelpOverviewCmd)
	{
		JXGetHelpManager()->ShowSection("OverviewHelp");
	}
	else if (index == kHelpWindowCmd)
	{
		JXGetHelpManager()->ShowSection(windowSectionName);
	}

	else if (index == kHelpChangeLogCmd)
	{
		JXGetHelpManager()->ShowChangeLog();
	}
	else if (index == kHelpCreditsCmd)
	{
		JXGetHelpManager()->ShowCredits();
	}
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
App::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);
	::CleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

	Required for call to WillBeMDIServer() *before* app is constructed.

 ******************************************************************************/

const JUtf8Byte*
App::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static public)

 ******************************************************************************/

void
App::InitStrings()
{
	JGetStringManager()->Register(kAppSignature, kDefaultStringData);
}
