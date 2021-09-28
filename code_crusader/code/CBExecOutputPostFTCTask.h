/******************************************************************************
 CBExecOutputPostFTCTask.h

	Copyright © 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBExecOutputPostFTCTask
#define _H_CBExecOutputPostFTCTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JBroadcaster.h>

class CBExecOutputDocument;

class CBExecOutputPostFTCTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	CBExecOutputPostFTCTask(CBExecOutputDocument* doc);

	virtual ~CBExecOutputPostFTCTask();

	virtual void	Perform();

private:

	CBExecOutputDocument*	itsDoc;		// not owned
};

#endif
