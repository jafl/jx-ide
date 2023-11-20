/******************************************************************************
 GetBreakpointsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetBreakpointsCmd
#define _H_JVMGetBreakpointsCmd

#include <GetBreakpointsCmd.h>

namespace jvm {

class GetBreakpointsCmd : public ::GetBreakpointsCmd
{
public:

	GetBreakpointsCmd();

	~GetBreakpointsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
