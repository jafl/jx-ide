/******************************************************************************
 GetCompletionsCmd.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GetCompletions
#define _H_GetCompletions

#include "Command.h"

class GetCompletionsCmd : public Command
{
public:

	GetCompletionsCmd(const JString& cmd);

	~GetCompletionsCmd() override;
};

#endif
