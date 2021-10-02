/******************************************************************************
 XDGetFrame.cpp

	BASE CLASS = GetFrameCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetFrame.h"
#include "StackWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetFrame::XDGetFrame
	(
	StackWidget* widget
	)
	:
	GetFrameCmd(JString("status", JString::kNoCopy)),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetFrame::~XDGetFrame()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetFrame::HandleSuccess
	(
	const JString& data
	)
{
	itsWidget->SelectFrame(0);
}
