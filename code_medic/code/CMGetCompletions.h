/******************************************************************************
 CMGetCompletions.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_CMGetCompletions
#define _H_CMGetCompletions

#include "CMCommand.h"

class CMGetCompletions : public CMCommand
{
public:

	CMGetCompletions(const JString& cmd);

	virtual	~CMGetCompletions();
};

#endif
