/******************************************************************************
 RunBackgroundCmdTask.cpp

	Let the event loop run between background commands.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/RunBackgroundCmdTask.h"
#include "lldb/Link.h"
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
	JXUrgentTask(cmd),
	itsCmd(cmd)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

lldb::RunBackgroundCmdTask::~RunBackgroundCmdTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
lldb::RunBackgroundCmdTask::Perform()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link != nullptr)
	{
		link->SendMedicCommandSync(itsCmd);
	}
}
