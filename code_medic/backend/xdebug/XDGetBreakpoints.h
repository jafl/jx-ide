/******************************************************************************
 XDGetBreakpoints.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetBreakpoints
#define _H_XDGetBreakpoints

#include "GetBreakpointsCmd.h"

class XDGetBreakpoints : public GetBreakpointsCmd
{
public:

	XDGetBreakpoints();

	virtual	~XDGetBreakpoints();

protected:

	void	HandleSuccess(const JString& data) override;
};

#endif
