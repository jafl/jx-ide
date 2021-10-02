/******************************************************************************
 GDBGetProgramName.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetProgramName
#define _H_GDBGetProgramName

#include "Command.h"

class GDBGetProgramName : public Command
{
public:

	GDBGetProgramName();

	virtual	~GDBGetProgramName();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
