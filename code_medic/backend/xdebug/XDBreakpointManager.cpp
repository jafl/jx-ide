/******************************************************************************
 XDBreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "XDBreakpointManager.h"
#include "XDGetBreakpoints.h"
#include "XDLink.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

XDBreakpointManager::XDBreakpointManager
	(
	XDLink* link
	)
	:
	BreakpointManager(link, jnew XDGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

XDBreakpointManager::~XDBreakpointManager()
{
}
