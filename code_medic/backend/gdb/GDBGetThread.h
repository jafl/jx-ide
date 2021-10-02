/******************************************************************************
 GDBGetThread.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetThread
#define _H_GDBGetThread

#include "GetThread.h"

class GDBGetThread : public GetThread
{
public:

	GDBGetThread(ThreadsWidget* widget);

	virtual	~GDBGetThread();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
