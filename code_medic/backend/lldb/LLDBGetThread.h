/******************************************************************************
 LLDBGetThread.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetThread
#define _H_LLDBGetThread

#include "GetThreadCmd.h"

class LLDBGetThread : public GetThreadCmd
{
public:

	LLDBGetThread(ThreadsWidget* widget);

	virtual	~LLDBGetThread();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
