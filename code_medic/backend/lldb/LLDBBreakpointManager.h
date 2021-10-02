/******************************************************************************
 LLDBBreakpointManager.h

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_LLDBBreakpointManager
#define _H_LLDBBreakpointManager

#include "BreakpointManager.h"

class LLDBLink;

class LLDBBreakpointManager : public BreakpointManager
{
public:

	LLDBBreakpointManager(LLDBLink* link);

	virtual	~LLDBBreakpointManager();
};

#endif
