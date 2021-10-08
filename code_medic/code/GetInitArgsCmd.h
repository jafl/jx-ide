/******************************************************************************
 GetInitArgsCmd.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetInitArgsCmd
#define _H_GetInitArgsCmd

#include "Command.h"

class GetInitArgsCmd : public Command
{
public:

	GetInitArgsCmd(const JString& cmd);

	~GetInitArgsCmd() override;
};

#endif
