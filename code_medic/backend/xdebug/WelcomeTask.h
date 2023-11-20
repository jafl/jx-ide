/******************************************************************************
 WelcomeTask.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDWelcomeTask
#define _H_XDWelcomeTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JString.h>

namespace xdebug {

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
