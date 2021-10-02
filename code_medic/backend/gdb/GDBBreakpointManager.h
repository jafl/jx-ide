/******************************************************************************
 GDBBreakpointManager.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_GDBBreakpointManager
#define _H_GDBBreakpointManager

#include "BreakpointManager.h"

class GDBLink;

class GDBBreakpointManager : public BreakpointManager
{
public:

	GDBBreakpointManager(GDBLink* link);

	virtual	~GDBBreakpointManager();
};

#endif
