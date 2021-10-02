/******************************************************************************
 JVMVarCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMVarCommand
#define _H_JVMVarCommand

#include "VarCommand.h"

class JVMVarCommand : public VarCommand
{
public:

	JVMVarCommand(const JString& cmd);

	virtual	~JVMVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
