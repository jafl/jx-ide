/******************************************************************************
 GetStopLocationForAsmCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocationForAsmCmd
#define _H_GDBGetStopLocationForAsmCmd

#include "gdb/GetStopLocationCmd.h"

namespace gdb {

class GetStopLocationForAsmCmd : public GetStopLocationCmd
{
public:

	GetStopLocationForAsmCmd();

	~GetStopLocationForAsmCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
