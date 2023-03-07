/******************************************************************************
 LLDBWelcomeTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBWelcomeTask
#define _H_LLDBWelcomeTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JString.h>

namespace lldb {

class WelcomeTask : public JXUrgentTask
{
public:

	WelcomeTask(const JString& msg, const bool restart);

protected:

	~WelcomeTask() override;

	void	Perform() override;

private:

	JString	itsMessage;
	bool	itsRestartFlag;
};

};

#endif
