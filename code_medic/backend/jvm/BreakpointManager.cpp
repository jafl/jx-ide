/******************************************************************************
 BreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#include "jvm/BreakpointManager.h"
#include "jvm/GetBreakpointsCmd.h"
#include "jvm/Link.h"
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
