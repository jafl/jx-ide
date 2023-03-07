/******************************************************************************
 WaitForSymbolUpdateTask.h

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_WaitForSymbolUpdateTask
#define _H_WaitForSymbolUpdateTask

#include <jx-af/jx/JXUrgentTask.h>

class JProcess;

class WaitForSymbolUpdateTask : public JXUrgentTask
{
public:

	WaitForSymbolUpdateTask(JProcess* p);

	void	StopWaiting();

protected:

	~WaitForSymbolUpdateTask() override;

	void	Perform() override;

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
