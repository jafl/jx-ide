/******************************************************************************
 LLDBRunBackgroundCommandTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBRunBackgroundCommandTask
#define _H_LLDBRunBackgroundCommandTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JBroadcaster.h>

class CMCommand;

class LLDBRunBackgroundCommandTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	LLDBRunBackgroundCommandTask(CMCommand* cmd);

	virtual ~LLDBRunBackgroundCommandTask();

	virtual void	Perform() override;

private:

	CMCommand*	itsCmd;
};

#endif
