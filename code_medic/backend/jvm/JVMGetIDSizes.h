/******************************************************************************
 JVMGetIDSizes.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetIDSizes
#define _H_JVMGetIDSizes

#include "Command.h"

class JVMGetIDSizes : public Command
{
public:

	JVMGetIDSizes();

	virtual	~JVMGetIDSizes();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
