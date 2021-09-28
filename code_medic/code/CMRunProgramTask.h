/******************************************************************************
 CMRunProgramTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMRunProgramTask
#define _H_CMRunProgramTask

#include <jx-af/jx/JXIdleTask.h>

class CMRunProgramTask : public JXIdleTask
{
public:

	CMRunProgramTask();

	virtual ~CMRunProgramTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);
};

#endif
