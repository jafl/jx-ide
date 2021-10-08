/******************************************************************************
 XDBreakpointManager.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_XDBreakpointManager
#define _H_XDBreakpointManager

#include "BreakpointManager.h"

class XDLink;

class XDBreakpointManager : public BreakpointManager
{
public:

	XDBreakpointManager(XDLink* link);

	~XDBreakpointManager();
};

#endif
