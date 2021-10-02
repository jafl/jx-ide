/******************************************************************************
 JVMGetThread.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThread
#define _H_JVMGetThread

#include "GetThread.h"

class JVMGetThread : public GetThread
{
public:

	JVMGetThread(ThreadsWidget* widget);

	virtual	~JVMGetThread();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
