/******************************************************************************
 LLDBEventTask.cpp

	Gives lldb time to process its events.

	BASE CLASS = JXIdleTask

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "LLDBEventTask.h"
#include "LLDBLink.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::EventTask::EventTask
	(
	Link* link
	)
	:
	JXIdleTask(0),
	itsLink(link)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::EventTask::~EventTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
lldb::EventTask::Perform
	(
	const Time delta
	)
{
	itsLink->HandleEvent();
}
