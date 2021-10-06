/******************************************************************************
 XDWelcomeTask.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDWelcomeTask
#define _H_XDWelcomeTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JString.h>

class XDWelcomeTask : public JXUrgentTask
{
public:

	XDWelcomeTask(const JString& msg, const bool error);

	virtual ~XDWelcomeTask();

	void	Perform() override;

private:

	JString	itsMessage;
	bool	itsErrorFlag;
};

#endif
