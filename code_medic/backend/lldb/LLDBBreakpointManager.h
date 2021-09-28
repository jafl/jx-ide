/******************************************************************************
 LLDBBreakpointManager.h

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_LLDBBreakpointManager
#define _H_LLDBBreakpointManager

#include "CMBreakpointManager.h"

class LLDBLink;

class LLDBBreakpointManager : public CMBreakpointManager
{
public:

	LLDBBreakpointManager(LLDBLink* link);

	virtual	~LLDBBreakpointManager();
};

#endif
