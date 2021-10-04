/******************************************************************************
 XDGetThread.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetThread
#define _H_XDGetThread

#include "GetThreadCmd.h"

class XDGetThread : public GetThreadCmd
{
public:

	XDGetThread(ThreadsWidget* widget);

	virtual	~XDGetThread();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
