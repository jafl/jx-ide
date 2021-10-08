/******************************************************************************
 LLDBGetBreakpointsCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetBreakpointsCmd
#define _H_LLDBGetBreakpointsCmd

#include "GetBreakpointsCmd.h"

namespace lldb {

class GetBreakpointsCmd : public ::GetBreakpointsCmd
{
public:

	GetBreakpointsCmd();

	~GetBreakpointsCmd();

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
