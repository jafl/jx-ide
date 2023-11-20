/******************************************************************************
 GetStopLocationForLinkCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocationForLinkCmd
#define _H_GDBGetStopLocationForLinkCmd

#include "gdb/GetStopLocationCmd.h"

namespace gdb {

class GetStopLocationForLinkCmd : public GetStopLocationCmd
{
public:

	GetStopLocationForLinkCmd();

	~GetStopLocationForLinkCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
