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

XDSetProgramTask::XDSetProgramTask()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDSetProgramTask::~XDSetProgramTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
XDSetProgramTask::Perform()
{
	auto* link = dynamic_cast<XDLink*>(GetLink());
	if (link != nullptr)
	{
		link->BroadcastProgramSet();
	}
}
