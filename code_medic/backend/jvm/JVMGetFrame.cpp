/******************************************************************************
 JVMGetFrame.cpp

	BASE CLASS = GetFrameCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetFrame.h"
#include "StackWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetFrame::JVMGetFrame
	(
	StackWidget* widget
	)
	:
	GetFrameCmd(JString("NOP", JString::kNoCopy)),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetFrame::~JVMGetFrame()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetFrame::HandleSuccess
	(
	const JString& data
	)
{
}
