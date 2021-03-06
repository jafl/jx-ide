/******************************************************************************
 XDGetThreadCmdsCmd.cpp

	BASE CLASS = GetThreadsCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetThreadsCmd.h"
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
	::GetThreadsCmd(JString("status", JString::kNoCopy), widget)
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
