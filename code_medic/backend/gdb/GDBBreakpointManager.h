/******************************************************************************
 GDBBreakpointManager.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_GDBBreakpointManager
#define _H_GDBBreakpointManager

#include "BreakpointManager.h"

namespace gdb {

class Link;

class BreakpointManager : public ::BreakpointManager
{
public:

	BreakpointManager(Link* link);

	virtual	~BreakpointManager();
};

};

#endif
