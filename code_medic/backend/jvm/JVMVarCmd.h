/******************************************************************************
 JVMVarCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMVarCmd
#define _H_JVMVarCmd

#include "VarCmd.h"

class JVMVarCmd : public VarCmd
{
public:

	JVMVarCmd(const JString& cmd);

	virtual	~JVMVarCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
