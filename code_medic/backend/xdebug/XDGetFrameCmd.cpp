/******************************************************************************
 XDGetFrameCmd.cpp

	BASE CLASS = GetFrameCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetFrameCmd.h"
#include "StackWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetFrameCmd::GetFrameCmd
	(
	StackWidget* widget
	)
	:
	::GetFrameCmd(JString("status", JString::kNoCopy)),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetFrameCmd::~GetFrameCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::GetFrameCmd::HandleSuccess
	(
	const JString& data
	)
{
	itsWidget->SelectFrame(0);
}
