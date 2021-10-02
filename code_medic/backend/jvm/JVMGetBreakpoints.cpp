/******************************************************************************
 JVMGetBreakpoints.cpp

	BASE CLASS = GetBreakpointsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetBreakpoints.h"
#include "JVMLink.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetBreakpoints::JVMGetBreakpoints()
	:
	GetBreakpointsCmd(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetBreakpoints::~JVMGetBreakpoints()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

void
JVMGetBreakpoints::HandleSuccess
	(
	const JString& data
	)
{
}
