/******************************************************************************
 GetBreakpointsCmd.cpp

	BASE CLASS = GetBreakpointsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/GetBreakpointsCmd.h"
#include "jvm/Link.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetBreakpointsCmd::GetBreakpointsCmd()
	:
	::GetBreakpointsCmd(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetBreakpointsCmd::~GetBreakpointsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

void
jvm::GetBreakpointsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
