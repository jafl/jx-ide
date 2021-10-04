/******************************************************************************
 JVMBreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#include "JVMBreakpointManager.h"
#include "JVMGetBreakpointsCmd.h"
#include "JVMLink.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

jvm::BreakpointManager::BreakpointManager
	(
	Link* link
	)
	:
	::BreakpointManager(link, jnew GetBreakpointsCmd())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

jvm::BreakpointManager::~BreakpointManager()
{
}
