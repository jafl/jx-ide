/******************************************************************************
 XDSetProgramTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "XDSetProgramTask.h"
#include "XDLink.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::SetProgramTask::SetProgramTask()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::SetProgramTask::~SetProgramTask()
{
}

/******************************************************************************
 Perform

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
