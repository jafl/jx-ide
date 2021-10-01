/******************************************************************************
 WaitForSymbolUpdateTask.h

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_WaitForSymbolUpdateTask
#define _H_WaitForSymbolUpdateTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JBroadcaster.h>

class JProcess;

class WaitForSymbolUpdateTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	WaitForSymbolUpdateTask(JProcess* p);

	virtual ~WaitForSymbolUpdateTask();

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
WaitForSymbolUpdateTask::StopWaiting()
{
	itsKeepWaitingFlag = false;
}

#endif
