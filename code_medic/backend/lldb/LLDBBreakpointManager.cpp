/******************************************************************************
 LLDBBreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#include "LLDBBreakpointManager.h"
#include "LLDBGetBreakpointsCmd.h"
#include "LLDBLink.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

lldb::BreakpointManager::BreakpointManager
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

lldb::BreakpointManager::~BreakpointManager()
{
}
