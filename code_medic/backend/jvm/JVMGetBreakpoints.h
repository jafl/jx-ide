/******************************************************************************
 JVMGetBreakpoints.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetBreakpoints
#define _H_JVMGetBreakpoints

#include "GetBreakpointsCmd.h"

class JVMGetBreakpoints : public GetBreakpointsCmd
{
public:

	JVMGetBreakpoints();

	virtual	~JVMGetBreakpoints();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
