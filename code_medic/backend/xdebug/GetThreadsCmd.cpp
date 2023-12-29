/******************************************************************************
 GetThreadCmdsCmd.cpp

	BASE CLASS = GetThreadsCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/GetThreadsCmd.h"
#include "ThreadsWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetThreadsCmd::GetThreadsCmd
	(
	JTree*			tree,
	ThreadsWidget*	widget
	)
	:
	::GetThreadsCmd("status", widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetThreadsCmd::~GetThreadsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::GetThreadsCmd::HandleSuccess
	(
	const JString& data
	)
{
	GetWidget()->FinishedLoading(0);
}
