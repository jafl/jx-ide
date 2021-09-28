/******************************************************************************
 XDBreakpointManager.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_XDBreakpointManager
#define _H_XDBreakpointManager

#include "CMBreakpointManager.h"

class XDLink;

class XDBreakpointManager : public CMBreakpointManager
{
public:

	XDBreakpointManager(XDLink* link);

	virtual	~XDBreakpointManager();
};

#endif
