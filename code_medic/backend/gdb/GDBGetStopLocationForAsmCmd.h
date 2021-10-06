/******************************************************************************
 GDBGetStopLocationForAsmCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocationForAsmCmd
#define _H_GDBGetStopLocationForAsmCmd

#include "GDBGetStopLocationCmd.h"

namespace gdb {

class GetStopLocationForAsmCmd : public GetStopLocationCmd
{
public:

	GetStopLocationForAsmCmd();

	virtual	~GetStopLocationForAsmCmd();

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
