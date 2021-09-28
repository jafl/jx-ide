/******************************************************************************
 CMQuitTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMQuitTask
#define _H_CMQuitTask

#include <jx-af/jx/JXIdleTask.h>

class CMQuitTask : public JXIdleTask
{
public:

	CMQuitTask();

	virtual ~CMQuitTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);
};

#endif
