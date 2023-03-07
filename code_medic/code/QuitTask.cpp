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
 Perform (virtual protected)

 ******************************************************************************/

void
QuitTask::Perform
	(
	const Time delta
	)
{
	bool quit = false;

	const JPtrArray<JXDirector>* list;
	if (JXGetApplication()->GetSubdirectors(&list))
	{
		quit = true;

		for (auto* d : *list)
		{
			if (d->IsActive())
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
