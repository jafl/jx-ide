/******************************************************************************
 WelcomeTask.cpp

	We cannot broadcast the Xdebug welcome message until everything has
	been created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/WelcomeTask.h"
#include "xdebug/Link.h"
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
	JXUrgentTask(GetApplication()),
	itsMessage(msg),
	itsErrorFlag(error)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

xdebug::WelcomeTask::~WelcomeTask()
{
}

/******************************************************************************
 Perform (virtual protected)

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
