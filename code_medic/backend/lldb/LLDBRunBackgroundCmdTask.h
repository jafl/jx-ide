/******************************************************************************
 LLDBRunBackgroundCmdTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBRunBackgroundCmdTask
#define _H_LLDBRunBackgroundCmdTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JBroadcaster.h>

class Command;

namespace lldb {

class RunBackgroundCmdTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	RunBackgroundCmdTask(Command* cmd);

	~RunBackgroundCmdTask() override;

	void	Perform() override;

private:

	Command*	itsCmd;
};

};

#endif
