/******************************************************************************
 GDBVarCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBVarCommand
#define _H_GDBVarCommand

#include "CMVarCommand.h"

class GDBVarCommand : public CMVarCommand
{
public:

	GDBVarCommand(const JString& cmd);

	virtual	~GDBVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif