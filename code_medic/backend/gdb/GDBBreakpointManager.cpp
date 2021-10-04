/******************************************************************************
 GDBBreakpointManager.cpp

	BASE CLASS = ::BreakpointManager

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "GDBBreakpointManager.h"
#include "GDBGetBreakpointsCmd.h"
#include "GDBLink.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

gdb::BreakpointManager::BreakpointManager
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

gdb::BreakpointManager::~BreakpointManager()
{
}
