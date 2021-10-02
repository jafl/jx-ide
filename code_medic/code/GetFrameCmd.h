/******************************************************************************
 GetFrameCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetFrameCmd
#define _H_GetFrameCmd

#include "Command.h"

class GetFrameCmd : public Command
{
public:

	GetFrameCmd(const JString& cmd);

	virtual	~GetFrameCmd();
};

#endif
