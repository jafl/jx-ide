/******************************************************************************
 LLDBBreakpointManager.h

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_LLDBBreakpointManager
#define _H_LLDBBreakpointManager

#include "BreakpointManager.h"

namespace lldb {

class Link;

class BreakpointManager : public ::BreakpointManager
{
public:

	BreakpointManager(Link* link);

	~BreakpointManager();
};

};

#endif
