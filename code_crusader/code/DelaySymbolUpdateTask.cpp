/******************************************************************************
 DelaySymbolUpdateTask.cpp

	BASE CLASS = JXIdleTask

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#include "DelaySymbolUpdateTask.h"
#include "ProjectDocument.h"
#include <jx-af/jcore/jAssert.h>

const Time kDelay = 60 * 1000;	// 60 seconds

/******************************************************************************
 Constructor

 ******************************************************************************/

DelaySymbolUpdateTask::DelaySymbolUpdateTask
	(
	ProjectDocument* projDoc
	)
	:
	JXIdleTask(kDelay),
	itsProjDoc(projDoc)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DelaySymbolUpdateTask::~DelaySymbolUpdateTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
DelaySymbolUpdateTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
	{
		itsProjDoc->UpdateSymbolDatabase();		// deletes us
	}
}
