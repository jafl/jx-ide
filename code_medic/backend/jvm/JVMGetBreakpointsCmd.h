/******************************************************************************
 JVMGetBreakpointsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetBreakpointsCmd
#define _H_JVMGetBreakpointsCmd

#include "GetBreakpointsCmd.h"

class JVMGetBreakpointsCmd : public GetBreakpointsCmd
{
public:

	JVMGetBreakpointsCmd();

	virtual	~JVMGetBreakpointsCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
