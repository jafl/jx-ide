/******************************************************************************
 WelcomeTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMWelcomeTask
#define _H_JVMWelcomeTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JString.h>

namespace jvm {

class WelcomeTask : public JXUrgentTask
{
public:

	WelcomeTask(const JString& msg, const bool error);

protected:

	~WelcomeTask() override;

	void	Perform() override;

private:

	JString	itsMessage;
	bool	itsErrorFlag;
};

};

#endif
