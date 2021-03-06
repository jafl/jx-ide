/******************************************************************************
 QuitTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_QuitTask
#define _H_QuitTask

#include <jx-af/jx/JXIdleTask.h>

class QuitTask : public JXIdleTask
{
public:

	QuitTask();

	~QuitTask() override;

	void	Perform(const Time delta, Time* maxSleepTime) override;
};

#endif
