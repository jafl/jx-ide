/******************************************************************************
 GetThread.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetThread.h"
#include "ThreadsWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetThread::GetThread
	(
	const JString&		cmd,
	ThreadsWidget*	widget
	)
	:
	Command(cmd, false, true),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetThread::~GetThread()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetThread::HandleFailure()
{
	itsWidget->FinishedLoading(0);
}
