/******************************************************************************
 LLDBGetRegistersCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetRegistersCmd
#define _H_LLDBGetRegistersCmd

#include "GetRegistersCmd.h"

namespace lldb {

class GetRegistersCmd : public ::GetRegistersCmd
{
public:

	GetRegistersCmd(RegistersDir* dir);

	~GetRegistersCmd();

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
