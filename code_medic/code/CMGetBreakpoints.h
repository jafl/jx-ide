/******************************************************************************
 CMGetBreakpoints.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetBreakpoints
#define _H_CMGetBreakpoints

#include "CMCommand.h"

class CMGetBreakpoints : public CMCommand
{
public:

	CMGetBreakpoints(const JString& cmd);

	virtual	~CMGetBreakpoints();
};

#endif
