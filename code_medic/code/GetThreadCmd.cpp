/******************************************************************************
 GetThreadCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetThreadCmd.h"
#include "ThreadsWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetThreadCmd::GetThreadCmd
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

GetThreadCmd::~GetThreadCmd()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetThreadCmd::HandleFailure()
{
	itsWidget->FinishedLoading(0);
}
