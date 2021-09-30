/******************************************************************************
 MDIServer.cpp

	<Description>

	BASE CLASS = public JXMDIServer

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#include "MDIServer.h"
#include "MainDirector.h"
#include "App.h"
#include "globals.h"
#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

MDIServer::MDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDIServer::~MDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
MDIServer::HandleMDIRequest
	(
	const JString&				dir,
	const JPtrArray<JString>&	argList
	)
{
	const JSize count = argList.GetElementCount();
	if (count <= 1)
	{
		JGetUserNotification()->ReportError(JGetString("MissingTemplate::MDIServer"));
		return;
	}

	for (JIndex i = 2; i <= count; i++)
	{
		JString arg	= *(argList.GetElement(i));
		if (arg == "--delete")
		{
			GetApplication()->ShouldBeDeletingTemplate(true);
		}
	}

	auto* dialog = jnew MainDirector(JXGetApplication(), argList);
	assert( dialog != nullptr );
	dialog->Activate();

	if (GetApplication()->IsDeletingTemplate())
	{
		for (JIndex i=2; i <= count; i++)
		{
			const JString& arg	= *(argList.GetElement(i));
			if (!arg.BeginsWith("-"))
			{
				JRemoveFile(arg);
			}
		}		
	}
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
MDIServer::PrintCommandLineHelp()
{
	const JUtf8Byte* map[] =
	{
		"vers", GetVersionNumberStr().GetBytes()
	};
	const JString s = JGetString("CommandLineHelp::MDIServer", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
