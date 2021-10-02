/******************************************************************************
 RunProgramTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_RunProgramTask
#define _H_RunProgramTask

#include <jx-af/jx/JXIdleTask.h>

class RunProgramTask : public JXIdleTask
{
public:

	RunProgramTask();

	virtual ~RunProgramTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);
};

#endif
