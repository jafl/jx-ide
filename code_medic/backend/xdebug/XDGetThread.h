/******************************************************************************
 XDGetThread.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetThread
#define _H_XDGetThread

#include "GetThread.h"

class XDGetThread : public GetThread
{
public:

	XDGetThread(ThreadsWidget* widget);

	virtual	~XDGetThread();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
