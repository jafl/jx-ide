/******************************************************************************
 GetThreads.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetThreads.h"
#include "ThreadsWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetThreads::GetThreads
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

GetThreads::~GetThreads()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetThreads::HandleFailure()
{
	itsWidget->FinishedLoading(0);
}
