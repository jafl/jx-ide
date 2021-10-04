/******************************************************************************
 JVMSetProgramTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMSetProgramTask.h"
#include "JVMLink.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::SetProgramTask::SetProgramTask()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::SetProgramTask::~SetProgramTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
jvm::SetProgramTask::Perform()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link != nullptr)
	{
		link->BroadcastProgramSet();
	}
}
