/******************************************************************************
 DisplaySourceForMainCmd.cpp

	Finds main() and displays it in the Current Source window.

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "DisplaySourceForMainCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DisplaySourceForMainCmd::DisplaySourceForMainCmd
	(
	SourceDirector*	sourceDir,
	const JString&		cmd
	)
	:
	Command(cmd, false, false),
	itsSourceDir(sourceDir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DisplaySourceForMainCmd::~DisplaySourceForMainCmd()
{
}
