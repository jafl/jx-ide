/******************************************************************************
 GetThread.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetThread
#define _H_GetThread

#include "Command.h"

class ThreadsWidget;

class GetThread : public Command
{
public:

	GetThread(const JString& cmd, ThreadsWidget* widget);

	virtual	~GetThread();

	ThreadsWidget*	GetWidget();

protected:

	virtual void	HandleFailure();

private:

	ThreadsWidget*	itsWidget;		// not owned
};


/******************************************************************************
 GetWidget

 ******************************************************************************/

inline ThreadsWidget*
GetThread::GetWidget()
{
	return itsWidget;
}

#endif
