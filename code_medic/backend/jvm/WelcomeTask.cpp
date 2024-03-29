/******************************************************************************
 WelcomeTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/WelcomeTask.h"
#include "jvm/Link.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::WelcomeTask::WelcomeTask
	(
	const JString&	msg,
	const bool		error
	)
	:
	JXUrgentTask(GetLink()),
	itsMessage(msg),
	itsErrorFlag(error)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

jvm::WelcomeTask::~WelcomeTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
jvm::WelcomeTask::Perform()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link != nullptr)
	{
		link->BroadcastWelcome(itsMessage, itsErrorFlag);
	}
}
