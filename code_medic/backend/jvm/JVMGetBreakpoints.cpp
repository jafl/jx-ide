/******************************************************************************
 JVMGetBreakpoints.cpp

	BASE CLASS = CMGetBreakpoints

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
	CMGetBreakpoints(JString("NOP", JString::kNoCopy))
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
