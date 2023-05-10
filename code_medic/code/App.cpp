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
#include "actionDefs.h"
#include "stringData.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXDockManager.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/JSimpleProcess.h>
#include <jx-af/jcore/JSubstitute.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jWebUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// Application signature (MDI, prefs)

static const JUtf8Byte* kAppSignature = "medic";

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"    About" 
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kHelpAboutCmd = 1,
	kHelpTOCCmd, kHelpOverviewCmd, kHelpWindowCmd,
	kHelpChangeLogCmd, kHelpCreditsCmd
};

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
	JXApplication(argc, argv, kAppSignature, kDefaultStringData)
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
	assert( task != nullptr );
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
			assert( dlog != nullptr );
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
 CreateHelpMenu

 ******************************************************************************/

#include <jx-af/image/jx/jx_help_toc.xpm>
#include <jx-af/image/jx/jx_help_specific.xpm>

JXTextMenu*
App::CreateHelpMenu
	(
	JXMenuBar*			menuBar,
	const JUtf8Byte*	idNamespace,
	const JUtf8Byte*	sectionName
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	menu->SetMenuItems(kHelpMenuStr, idNamespace);
	menu->SetUpdateAction(JXMenu::kDisableNone);

	menu->SetItemImage(kHelpTOCCmd,    jx_help_toc);
	menu->SetItemImage(kHelpWindowCmd, jx_help_specific);

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
