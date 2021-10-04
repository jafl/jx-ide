/******************************************************************************
 JVMGetThreadCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadCmd
#define _H_JVMGetThreadCmd

#include "GetThreadCmd.h"

class JVMGetThreadCmd : public GetThreadCmd
{
public:

	JVMGetThreadCmd(ThreadsWidget* widget);

	virtual	~JVMGetThreadCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
