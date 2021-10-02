/******************************************************************************
 GDBCheckCoreStatusCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBCheckCoreStatusCmd
#define _H_GDBCheckCoreStatusCmd

#include "Command.h"

class GDBCheckCoreStatusCmd : public Command
{
public:

	GDBCheckCoreStatusCmd();

	virtual	~GDBCheckCoreStatusCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
