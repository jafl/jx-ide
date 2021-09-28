/******************************************************************************
 CBWaitForSymbolUpdateTask.h

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBWaitForSymbolUpdateTask
#define _H_CBWaitForSymbolUpdateTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JBroadcaster.h>

class JProcess;

class CBWaitForSymbolUpdateTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	CBWaitForSymbolUpdateTask(JProcess* p);

	virtual ~CBWaitForSymbolUpdateTask();

	virtual void	Perform();

	void	StopWaiting();

private:

	JProcess*	itsProcess;
	bool		itsKeepWaitingFlag;
};


/******************************************************************************
 StopWaiting

 ******************************************************************************/

inline void
CBWaitForSymbolUpdateTask::StopWaiting()
{
	itsKeepWaitingFlag = false;
}

#endif
