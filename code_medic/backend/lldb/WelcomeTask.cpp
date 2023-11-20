/******************************************************************************
 WelcomeTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/WelcomeTask.h"
#include "lldb/Link.h"
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
	JXUrgentTask(GetApplication()),
	itsMessage(msg),
	itsRestartFlag(restart)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

lldb::WelcomeTask::~WelcomeTask()
{
}

/******************************************************************************
 Perform (virtual protected)

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
