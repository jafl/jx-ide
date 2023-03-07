/******************************************************************************
 main.cpp

	Main program for Code Crusader.

	Copyright © 1996-2000 by John Lindal.

 ******************************************************************************/

#include "MDIServer.h"
#include "TextDocument.h"
#include "globals.h"
#include <jx-af/jx/JXDockManager.h>
#include <jx-af/jx/JXSplashWindow.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/jCommandLine.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jWebUtil.h>
#include <jx-af/jcore/jSysUtil.h>
#include <jx-af/jcore/jAssert.h>

// Prototypes

void ParseTextOptions(const JSize argc, char* argv[], JString* commitFile);
void PrintVersion();

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	JString commitFile;
	ParseTextOptions(argc, argv, &commitFile);

	const bool useMDI = commitFile.IsEmpty();
	if (useMDI && !MDIServer::WillBeMDIServer(App::GetAppSignature(), argc, argv))
	{
		return 0;
	}

	bool displayAbout;
	JString prevVersStr;
	auto* app = jnew App(&argc, argv, useMDI, &displayAbout, &prevVersStr);
	assert( app != nullptr );

	JXApplication::StartFiber([argc, argv, commitFile]()
	{
		MDIServer* mdi;
		if (GetMDIServer(&mdi))
		{
			mdi->HandleCmdLineOptions(argc, argv);
		}

		if (!commitFile.IsEmpty())
		{
			JXWindow::ShouldAutoDockNewWindows(false);

			DocumentManager* docMgr = GetDocumentManager();
			docMgr->OpenSomething(commitFile);

			JPtrArray<TextDocument>* docList = docMgr->GetTextDocList();
			if (!docList->IsEmpty())
			{
				docList->GetFirstElement()->ShouldMakeBackupFile(false);
			}
		}
	});

	if (displayAbout)
	{
		app->DisplayAbout(true, prevVersStr);
	}
	else if (useMDI)
	{
		JCheckForNewerVersion(GetPrefsManager(), kVersionCheckID);
	}

	app->Run();
	return 0;
}

/******************************************************************************
 ParseTextOptions

	Handle the command line options that don't require opening an X display.

	Options that don't call exit() should use JXApplication::RemoveCmdLineOption()
	so ParseXOptions won't gag.

 ******************************************************************************/

void
ParseTextOptions
	(
	const JSize	argc,
	char*		argv[],
	JString*	commitFile
	)
{
	commitFile->Clear();

	JIndex index = 1;
	while (index < argc)
	{
		if (JIsVersionRequest(argv[index]))
		{
			App::InitStrings();
			PrintVersion();
			exit(0);
		}
		else if (JIsHelpRequest(argv[index]))
		{
			App::InitStrings();
			MDIServer::PrintCommandLineHelp();
			exit(0);
		}

		else if (strcmp(argv[index], "--vcs-commit") == 0 ||
				 strcmp(argv[index], "--cvs-commit") == 0)	// legacy
		{
			JCheckForValues(1, &index, argc, argv);
			*commitFile = argv[index];
		}

		index++;
	}
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	std::cout << GetVersionStr() << std::endl;
	std::cout << std::endl;
}
