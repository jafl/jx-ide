/******************************************************************************
 GetLocalVarsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetLocalVarsCmd
#define _H_GetLocalVarsCmd

#include "Command.h"

class GetLocalVarsCmd : public Command
{
public:

	GetLocalVarsCmd(const JString& cmd);

	~GetLocalVarsCmd();
};

#endif
