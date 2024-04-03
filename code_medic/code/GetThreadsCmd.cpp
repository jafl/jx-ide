/******************************************************************************
 GetThreadsCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetThreadsCmd.h"
#include "ThreadsWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetThreadsCmd::GetThreadsCmd
	(
	const JString&	cmd,
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

GetThreadsCmd::~GetThreadsCmd()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetThreadsCmd::HandleFailure()
{
	itsWidget->FinishedLoading(0);
}
