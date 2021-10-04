/******************************************************************************
 JVMGetClassMethodsCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetClassMethodsCmd
#define _H_JVMGetClassMethodsCmd

#include "Command.h"

class JVMGetClassMethodsCmd : public Command
{
public:

	JVMGetClassMethodsCmd(const JUInt64 id);

	virtual	~JVMGetClassMethodsCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	const JUInt64	itsID;
};

#endif
