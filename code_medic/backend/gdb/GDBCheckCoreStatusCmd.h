/******************************************************************************
 GDBCheckCoreStatusCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBCheckCoreStatusCmd
#define _H_GDBCheckCoreStatusCmd

#include "Command.h"

namespace gdb {

class CheckCoreStatusCmd : public Command
{
public:

	CheckCoreStatusCmd();

	~CheckCoreStatusCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
