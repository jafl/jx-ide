/******************************************************************************
 GDBGetRegistersCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetRegistersCmd
#define _H_GDBGetRegistersCmd

#include "GetRegistersCmd.h"

namespace gdb {

class GetRegistersCmd : public ::GetRegistersCmd
{
public:

	GetRegistersCmd(RegistersDir* dir);

	virtual	~GetRegistersCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

};

#endif
