/******************************************************************************
 GDBPingTask.cpp

	On macOS, we have to periodically ping gdb to see if it is available,
	because it does not notify us of state changes.

	BASE CLASS = JXIdleTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "GDBPingTask.h"
#include "GDBLink.h"
#include "globals.h"

const Time kCheckInterval = 1000;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::PingTask::PingTask()
	:
	JXIdleTask(kCheckInterval)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::PingTask::~PingTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
gdb::PingTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
	{
		dynamic_cast<gdb::Link*>(GetLink())->SendPing();
	}
}
