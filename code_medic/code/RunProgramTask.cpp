/******************************************************************************
 RunProgramTask.cpp

	We cannot run immediately after the program is selected or even when we
	receive kSymbolsLoaded.  We can only run after everybody else has
	finished processing kSymbolsLoaded.

	BASE CLASS = JXIdleTask

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "RunProgramTask.h"
#include "CommandDirector.h"
#include "globals.h"

const Time kCheckInterval = 1;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

RunProgramTask::RunProgramTask()
	:
	JXIdleTask(kCheckInterval)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RunProgramTask::~RunProgramTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
RunProgramTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (!GetLink()->HasPendingCommands())
	{
		(GetCommandDirector())->RunProgram();
		jdelete this;
	}
}
