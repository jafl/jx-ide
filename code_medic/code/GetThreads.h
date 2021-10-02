/******************************************************************************
 GetThreads.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetThreads
#define _H_GetThreads

#include "Command.h"

class ThreadsWidget;

class GetThreads : public Command
{
public:

	GetThreads(const JString& cmd, ThreadsWidget* widget);

	virtual	~GetThreads();

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
GetThreads::GetWidget()
{
	return itsWidget;
}

#endif
