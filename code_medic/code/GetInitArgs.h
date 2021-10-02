/******************************************************************************
 GetInitArgs.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetInitArgs
#define _H_GetInitArgs

#include "Command.h"

class GetInitArgs : public Command
{
public:

	GetInitArgs(const JString& cmd);

	virtual	~GetInitArgs();
};

#endif
