/******************************************************************************
 GetProgramNameCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetProgramNameCmd
#define _H_GDBGetProgramNameCmd

#include "Command.h"

namespace gdb {

class GetProgramNameCmd : public Command
{
public:

	GetProgramNameCmd();

	~GetProgramNameCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
