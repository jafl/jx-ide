/******************************************************************************
 LLDBWelcomeTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBWelcomeTask.h"
#include "LLDBLink.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::WelcomeTask::WelcomeTask
	(
	const JString&	msg,
	const bool		restart
	)
	:
	itsMessage(msg),
	itsRestartFlag(restart)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::WelcomeTask::~WelcomeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
lldb::WelcomeTask::Perform()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link != nullptr)
	{
		link->BroadcastWelcome(itsMessage, itsRestartFlag);
	}
}
