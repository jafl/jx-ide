/******************************************************************************
 CommandOutputPostFTCTask.h

	Copyright © 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_CommandOutputPostFTCTask
#define _H_CommandOutputPostFTCTask

#include <jx-af/jx/JXUrgentTask.h>

class CommandOutputDocument;

class CommandOutputPostFTCTask : public JXUrgentTask
{
public:

	CommandOutputPostFTCTask(CommandOutputDocument* doc);

protected:

	~CommandOutputPostFTCTask() override;

	void	Perform() override;

private:

	CommandOutputDocument*	itsDoc;		// not owned
};

#endif
