/******************************************************************************
 LLDBGetThread.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetThread
#define _H_LLDBGetThread

#include "GetThread.h"

class LLDBGetThread : public GetThread
{
public:

	LLDBGetThread(ThreadsWidget* widget);

	virtual	~LLDBGetThread();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
