/******************************************************************************
 SetProgramTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "xdebug/SetProgramTask.h"
#include "xdebug/Link.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::SetProgramTask::SetProgramTask()
	:
	JXUrgentTask(GetLink())
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

xdebug::SetProgramTask::~SetProgramTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
xdebug::SetProgramTask::Perform()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link != nullptr)
	{
		link->BroadcastProgramSet();
	}
}
