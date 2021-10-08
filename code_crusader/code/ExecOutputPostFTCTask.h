/******************************************************************************
 ExecOutputPostFTCTask.h

	Copyright © 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_ExecOutputPostFTCTask
#define _H_ExecOutputPostFTCTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JBroadcaster.h>

class ExecOutputDocument;

class ExecOutputPostFTCTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	ExecOutputPostFTCTask(ExecOutputDocument* doc);

	~ExecOutputPostFTCTask();

	void	Perform() override;

private:

	ExecOutputDocument*	itsDoc;		// not owned
};

#endif
