/******************************************************************************
 GetFrameCmd.cpp

	BASE CLASS = GetFrameCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/GetFrameCmd.h"
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
	::GetFrameCmd("NOP"),
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
