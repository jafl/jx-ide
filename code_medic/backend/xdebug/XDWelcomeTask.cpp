/******************************************************************************
 XDWelcomeTask.cpp

	We cannot broadcast the Xdebug welcome message until everything has
	been created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDWelcomeTask.h"
#include "XDLink.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::WelcomeTask::WelcomeTask
	(
	const JString&	msg,
	const bool		error
	)
	:
	itsMessage(msg),
	itsErrorFlag(error)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::WelcomeTask::~WelcomeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
xdebug::WelcomeTask::Perform()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link != nullptr)
	{
		link->BroadcastWelcome(itsMessage, itsErrorFlag);
	}
}
