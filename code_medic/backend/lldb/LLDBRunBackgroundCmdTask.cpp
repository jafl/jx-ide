/******************************************************************************
 LLDBRunBackgroundCmdTask.cpp

	Let the event loop run between background commands.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBRunBackgroundCmdTask.h"
#include "LLDBLink.h"
#include "Command.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::RunBackgroundCmdTask::RunBackgroundCmdTask
	(
	Command* cmd
	)
	:
	itsCmd(cmd)
{
	ClearWhenGoingAway(itsCmd, &itsCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::RunBackgroundCmdTask::~RunBackgroundCmdTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
lldb::RunBackgroundCmdTask::Perform()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link != nullptr && itsCmd != nullptr)
	{
		link->SendMedicCommandSync(itsCmd);
	}
}
