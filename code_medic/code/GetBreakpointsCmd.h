/******************************************************************************
 GetBreakpointsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetBreakpoints
#define _H_GetBreakpoints

#include "Command.h"

class GetBreakpointsCmd : public Command
{
public:

	GetBreakpointsCmd(const JString& cmd);

	~GetBreakpointsCmd() override;
};

#endif
