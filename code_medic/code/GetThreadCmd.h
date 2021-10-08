/******************************************************************************
 GetThreadCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetThreadCmd
#define _H_GetThreadCmd

#include "Command.h"

class ThreadsWidget;

class GetThreadCmd : public Command
{
public:

	GetThreadCmd(const JString& cmd, ThreadsWidget* widget);

	~GetThreadCmd() override;

	ThreadsWidget*	GetWidget();

protected:

	void	HandleFailure() override;

private:

	ThreadsWidget*	itsWidget;		// not owned
};


/******************************************************************************
 GetWidget

 ******************************************************************************/

inline ThreadsWidget*
GetThreadCmd::GetWidget()
{
	return itsWidget;
}

#endif
