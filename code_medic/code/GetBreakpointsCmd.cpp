/******************************************************************************
 GetBreakpointsCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetBreakpointsCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetBreakpointsCmd::GetBreakpointsCmd
	(
	const JString& cmd
	)
	:
	Command(cmd, false, false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetBreakpointsCmd::~GetBreakpointsCmd()
{
}
