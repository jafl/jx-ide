/******************************************************************************
 GetThreadCmd.cpp

	BASE CLASS = GetThreadCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/GetThreadCmd.h"
#include "ThreadsWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetThreadCmd::GetThreadCmd
	(
	ThreadsWidget* widget
	)
	:
	::GetThreadCmd("status", widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetThreadCmd::~GetThreadCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::GetThreadCmd::HandleSuccess
	(
	const JString& data
	)
{
	GetWidget()->FinishedLoading(0);
}
