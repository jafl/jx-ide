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

protected:

	void	Perform(const Time delta) override;
};

#endif
