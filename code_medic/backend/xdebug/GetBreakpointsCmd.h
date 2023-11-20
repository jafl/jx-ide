/******************************************************************************
 GetBreakpointsCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetBreakpointsCmd
#define _H_XDGetBreakpointsCmd

#include <GetBreakpointsCmd.h>

namespace xdebug {

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
