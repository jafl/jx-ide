/******************************************************************************
 BreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#include "lldb/BreakpointManager.h"
#include "lldb/GetBreakpointsCmd.h"
#include "lldb/Link.h"
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
