/******************************************************************************
 XDWelcomeTask.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDWelcomeTask
#define _H_XDWelcomeTask

#include <JXUrgentTask.h>
#include <JString.h>

class XDWelcomeTask : public JXUrgentTask
{
public:

	XDWelcomeTask(const JString& msg, const bool error);

	virtual ~XDWelcomeTask();

	virtual void	Perform() override;

private:

	JString	itsMessage;
	bool	itsErrorFlag;
};

#endif
