/******************************************************************************
 App.cpp

	BASE CLASS = JXApplication

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "App.h"
#include "MDIServer.h"
#include "AboutDialog.h"
#include "QuitTask.h"
#include "lldb/LLDBLink.h"
#include "globals.h"
#include "stringData.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXAskInitDockAll.h>
#include <jx-af/jcore/JSimpleProcess.h>
#include <jx-af/jcore/JSubstitute.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
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
	bool*	displayAbout,
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
	const JString&	prevVersStr,
	const bool	init
	)
{
	auto* dlog = jnew AboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();

	if (init && prevVersStr.IsEmpty())
	{
		auto* task = jnew JXAskInitDockAll(dlog);
		assert( task != nullptr );
		task->Start();
	}
}

/******************************************************************************
 HandleCustomEvent (virtual protected)

 ******************************************************************************/

bool
App::HandleCustomEvent()
{
	auto* link = dynamic_cast<lldb::Link*>(GetLink());
	if (link != nullptr)
	{
		link->HandleEvent();
	}
	return true;
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
