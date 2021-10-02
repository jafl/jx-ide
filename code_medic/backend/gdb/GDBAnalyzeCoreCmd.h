/******************************************************************************
 GDBAnalyzeCoreCmd.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBAnalyzeCoreCmd
#define _H_GDBAnalyzeCoreCmd

#include "Command.h"

class GDBAnalyzeCoreCmd : public Command
{
public:

	GDBAnalyzeCoreCmd(const JString& cmd);

	virtual	~GDBAnalyzeCoreCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
