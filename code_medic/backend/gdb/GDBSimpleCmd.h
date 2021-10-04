/******************************************************************************
 GDBSimpleCmd.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBSimpleCmd
#define _H_GDBSimpleCmd

#include "Command.h"

namespace gdb {

class SimpleCmd : public Command
{
public:

	SimpleCmd(const JString& cmd);

	virtual	~SimpleCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

};

#endif
