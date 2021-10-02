/******************************************************************************
 JVMBreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#include "JVMBreakpointManager.h"
#include "JVMGetBreakpoints.h"
#include "JVMLink.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JVMBreakpointManager::JVMBreakpointManager
	(
	JVMLink* link
	)
	:
	BreakpointManager(link, jnew JVMGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JVMBreakpointManager::~JVMBreakpointManager()
{
}
