/******************************************************************************
 GDBPingTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBPingTask
#define _H_GDBPingTask

#include <jx-af/jx/JXIdleTask.h>

class GDBPingTask : public JXIdleTask
{
public:

	GDBPingTask();

	virtual ~GDBPingTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime) override;
};

#endif
