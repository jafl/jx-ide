/******************************************************************************
 JVMGetIDSizesCmd.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetIDSizesCmd
#define _H_JVMGetIDSizesCmd

#include "Command.h"

class JVMGetIDSizesCmd : public Command
{
public:

	JVMGetIDSizesCmd();

	virtual	~JVMGetIDSizesCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
