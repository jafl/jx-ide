/******************************************************************************
 LLDBBreakpointManager.cpp

	BASE CLASS = BreakpointManager

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#include "LLDBBreakpointManager.h"
#include "LLDBGetBreakpoints.h"
#include "LLDBLink.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

LLDBBreakpointManager::LLDBBreakpointManager
	(
	LLDBLink* link
	)
	:
	BreakpointManager(link, jnew LLDBGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LLDBBreakpointManager::~LLDBBreakpointManager()
{
}
