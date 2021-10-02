/******************************************************************************
 GDBBreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "GDBBreakpointManager.h"
#include "GDBGetBreakpointsCmd.h"
#include "GDBLink.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBBreakpointManager::GDBBreakpointManager
	(
	GDBLink* link
	)
	:
	BreakpointManager(link, jnew GDBGetBreakpointsCmd())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBBreakpointManager::~GDBBreakpointManager()
{
}
