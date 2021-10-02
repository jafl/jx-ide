/******************************************************************************
 LLDBGetBreakpoints.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetBreakpoints
#define _H_LLDBGetBreakpoints

#include "GetBreakpointsCmd.h"

class LLDBGetBreakpoints : public GetBreakpointsCmd
{
public:

	LLDBGetBreakpoints();

	virtual	~LLDBGetBreakpoints();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
