/******************************************************************************
 BreakpointManager.cpp

	BASE CLASS = ::BreakpointManager

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "gdb/BreakpointManager.h"
#include "gdb/GetBreakpointsCmd.h"
#include "gdb/Link.h"
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
