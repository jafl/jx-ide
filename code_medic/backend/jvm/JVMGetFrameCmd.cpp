/******************************************************************************
 JVMGetFrameCmd.cpp

	BASE CLASS = GetFrameCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetFrameCmd.h"
#include "StackWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetFrameCmd::JVMGetFrameCmd
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

JVMGetFrameCmd::~JVMGetFrameCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetFrameCmd::HandleSuccess
	(
	const JString& data
	)
{
}
