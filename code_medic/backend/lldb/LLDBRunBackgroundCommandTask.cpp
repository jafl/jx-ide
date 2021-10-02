/******************************************************************************
 LLDBRunBackgroundCommandTask.cpp

	Let the event loop run between background commands.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBRunBackgroundCommandTask.h"
#include "LLDBLink.h"
#include "Command.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBRunBackgroundCommandTask::LLDBRunBackgroundCommandTask
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

LLDBRunBackgroundCommandTask::~LLDBRunBackgroundCommandTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
LLDBRunBackgroundCommandTask::Perform()
{
	auto* link = dynamic_cast<LLDBLink*>(GetLink());
	if (link != nullptr && itsCmd != nullptr)
	{
		link->SendMedicCommandSync(itsCmd);
	}
}
