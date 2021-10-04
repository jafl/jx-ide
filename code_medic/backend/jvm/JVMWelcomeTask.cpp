/******************************************************************************
 JVMWelcomeTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMWelcomeTask.h"
#include "JVMLink.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::WelcomeTask::WelcomeTask
	(
	const JString&	msg,
	const bool	error
	)
	:
	itsMessage(msg),
	itsErrorFlag(error)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::WelcomeTask::~WelcomeTask()
{
}

/******************************************************************************
 Perform

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
