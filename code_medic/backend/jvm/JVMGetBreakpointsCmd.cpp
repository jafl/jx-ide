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

JVMGetBreakpointsCmd::JVMGetBreakpointsCmd()
	:
	GetBreakpointsCmd(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetBreakpointsCmd::~JVMGetBreakpointsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

void
JVMGetBreakpointsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
