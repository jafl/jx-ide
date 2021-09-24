/******************************************************************************
 JVMGetThread.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThread
#define _H_JVMGetThread

#include "CMGetThread.h"

class JVMGetThread : public CMGetThread
{
public:

	JVMGetThread(CMThreadsWidget* widget);

	virtual	~JVMGetThread();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
