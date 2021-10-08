/******************************************************************************
 GDBGetStopLocationForLinkCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocationForLinkCmd
#define _H_GDBGetStopLocationForLinkCmd

#include "GDBGetStopLocationCmd.h"

namespace gdb {

class GetStopLocationForLinkCmd : public GetStopLocationCmd
{
public:

	GetStopLocationForLinkCmd();

	~GetStopLocationForLinkCmd();

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
