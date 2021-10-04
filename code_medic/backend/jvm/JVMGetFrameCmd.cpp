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

jvm::GetFrameCmd::GetFrameCmd
	(
	StackWidget* widget
	)
	:
	::GetFrameCmd(JString("NOP", JString::kNoCopy)),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetFrameCmd::~GetFrameCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetFrameCmd::HandleSuccess
	(
	const JString& data
	)
{
}
