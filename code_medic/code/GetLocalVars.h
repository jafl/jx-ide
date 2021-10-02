/******************************************************************************
 GetLocalVars.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetLocalVars
#define _H_GetLocalVars

#include "Command.h"

class GetLocalVars : public Command
{
public:

	GetLocalVars(const JString& cmd);

	virtual	~GetLocalVars();
};

#endif
