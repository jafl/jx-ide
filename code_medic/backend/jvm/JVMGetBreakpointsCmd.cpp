/******************************************************************************
 JVMGetBreakpointsCmd.cpp

	BASE CLASS = GetBreakpointsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetBreakpointsCmd.h"
#include "JVMLink.h"
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
