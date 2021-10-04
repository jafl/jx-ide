/******************************************************************************
 XDGetThreads.cpp

	BASE CLASS = GetThreadsCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetThreads.h"
#include "ThreadsWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetThreads::XDGetThreads
	(
	JTree*				tree,
	ThreadsWidget*	widget
	)
	:
	GetThreadsCmd(JString("status", JString::kNoCopy), widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetThreads::~XDGetThreads()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetThreads::HandleSuccess
	(
	const JString& data
	)
{
	GetWidget()->FinishedLoading(0);
}
