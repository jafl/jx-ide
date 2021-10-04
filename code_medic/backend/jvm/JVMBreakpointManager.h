/******************************************************************************
 JVMBreakpointManager.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_JVMBreakpointManager
#define _H_JVMBreakpointManager

#include "BreakpointManager.h"

namespace jvm {

class Link;
class GetBreakpointsCmd;

class BreakpointManager : public ::BreakpointManager
{
public:

	BreakpointManager(Link* link);

	virtual	~BreakpointManager();
};

};

#endif
