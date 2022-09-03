/******************************************************************************
 QuitTask.cpp

	Quit if all windows are hidden.

	BASE CLASS = JXIdleTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "QuitTask.h"
#include "CommandDirector.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

const Time kCheckInterval = 1000;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

QuitTask::QuitTask()
	:
	JXIdleTask(kCheckInterval)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

QuitTask::~QuitTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
QuitTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	bool quit = false;

	const JPtrArray<JXDirector>* list;
	if (TimeToPerform(delta, maxSleepTime) &&
		JXGetApplication()->GetSubdirectors(&list))
	{
		quit = true;

		const JSize count = list->GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			if (list->GetElement(i)->IsActive())
			{
				quit = false;
				break;
			}
		}
	}

	if (quit && GetLink()->ProgramIsRunning())
	{
		GetCommandDirector()->Activate();
	}
	else if (quit)
	{
		JXGetApplication()->Quit();
		jdelete this;
	}
}
