/******************************************************************************
 JVMBreakpointManager.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_JVMBreakpointManager
#define _H_JVMBreakpointManager

#include "BreakpointManager.h"

class JVMLink;
class JVMGetBreakpoints;

class JVMBreakpointManager : public BreakpointManager
{
public:

	JVMBreakpointManager(JVMLink* link);

	virtual	~JVMBreakpointManager();
};

#endif
