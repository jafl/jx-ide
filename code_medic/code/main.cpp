/******************************************************************************
 main.cpp

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "App.h"
#include "CommandDirector.h"
#include "MDIServer.h"
#include "globals.h"
#include <jx-af/jx/JXDockManager.h>
#include <jx-af/jx/JXSplashWindow.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jCommandLine.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jWebUtil.h>
#include <jx-af/jcore/jSysUtil.h>
#include <jx-af/jcore/jAssert.h>

// Prototypes

void ParseTextOptions(const int argc, char* argv[]);

void PrintHelp();
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
	ParseTextOptions(argc, argv);

	if (!MDIServer::WillBeMDIServer(App::GetAppSignature(), argc, argv))
	{
		return 0;
	}

	bool displayAbout;
	JString prevVersStr;
	auto* app = jnew App(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != nullptr );

	CreateCommandDirector();	// so dock appears after JCheckForNewerVersion()

	JXApplication::StartFiber([argc, argv]()
	{
		JXMDIServer* mdi;
		if (JXGetMDIServer(&mdi))
		{
			mdi->HandleCmdLineOptions(argc, argv);
		}
	});

	if (displayAbout)
	{
		app->DisplayAbout(true, prevVersStr);
	}
	else
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
	const int	argc,
	char*		argv[]
	)
{
	long index = 1;
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
			PrintHelp();
			exit(0);
		}
		index++;
	}
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp()
{
	const JUtf8Byte* map[] =
	{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	const JString s = JGetString("CommandLineHelp", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	const JUtf8Byte* map[] =
	{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	std::cout << JGetString("Description", map, sizeof(map));
	std::cout << std::endl << std::endl;
}
