/******************************************************************************
 XDGetThread.cpp

	BASE CLASS = GetThreadCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetThread.h"
#include "ThreadsWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetThread::XDGetThread
	(
	ThreadsWidget* widget
	)
	:
	GetThreadCmd(JString("status", JString::kNoCopy), widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetThread::~XDGetThread()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetThread::HandleSuccess
	(
	const JString& data
	)
{
	GetWidget()->FinishedLoading(0);
}
