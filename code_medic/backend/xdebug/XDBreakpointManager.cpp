/******************************************************************************
 XDBreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "XDBreakpointManager.h"
#include "XDGetBreakpointsCmd.h"
#include "XDLink.h"
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
