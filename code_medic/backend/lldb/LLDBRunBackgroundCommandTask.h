/******************************************************************************
 LLDBRunBackgroundCommandTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBRunBackgroundCommandTask
#define _H_LLDBRunBackgroundCommandTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JBroadcaster.h>

class Command;

class LLDBRunBackgroundCommandTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	LLDBRunBackgroundCommandTask(Command* cmd);

	virtual ~LLDBRunBackgroundCommandTask();

	void	Perform() override;

private:

	Command*	itsCmd;
};

#endif
