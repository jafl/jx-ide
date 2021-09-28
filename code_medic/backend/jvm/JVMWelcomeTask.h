/******************************************************************************
 JVMWelcomeTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMWelcomeTask
#define _H_JVMWelcomeTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JString.h>

class JVMWelcomeTask : public JXUrgentTask
{
public:

	JVMWelcomeTask(const JString& msg, const bool error);

	virtual ~JVMWelcomeTask();

	virtual void	Perform();

private:

	JString		itsMessage;
	bool	itsErrorFlag;
};

#endif
