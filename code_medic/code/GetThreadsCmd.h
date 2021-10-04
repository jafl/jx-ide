/******************************************************************************
 GetThreadsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetThreadsCmd
#define _H_GetThreadsCmd

#include "Command.h"

class ThreadsWidget;

class GetThreadsCmd : public Command
{
public:

	GetThreadsCmd(const JString& cmd, ThreadsWidget* widget);

	virtual	~GetThreadsCmd();

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
GetThreadsCmd::GetWidget()
{
	return itsWidget;
}

#endif
