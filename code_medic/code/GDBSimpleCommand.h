/******************************************************************************
 GDBSimpleCommand.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBSimpleCommand
#define _H_GDBSimpleCommand

#include "CMCommand.h"

class GDBSimpleCommand : public CMCommand
{
public:

	GDBSimpleCommand(const JString& cmd);

	virtual	~GDBSimpleCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
