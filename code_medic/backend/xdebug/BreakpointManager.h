/******************************************************************************
 BreakpointManager.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_XDBreakpointManager
#define _H_XDBreakpointManager

#include <BreakpointManager.h>

namespace xdebug {

class Link;

class BreakpointManager : public ::BreakpointManager
{
public:

	BreakpointManager(Link* link);

	~BreakpointManager() override;
};

};

#endif
