/******************************************************************************
 BreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "xdebug/BreakpointManager.h"
#include "xdebug/GetBreakpointsCmd.h"
#include "xdebug/Link.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

xdebug::BreakpointManager::BreakpointManager
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

xdebug::BreakpointManager::~BreakpointManager()
{
}
