/******************************************************************************
 LLDBWelcomeTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBWelcomeTask
#define _H_LLDBWelcomeTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JString.h>

class LLDBWelcomeTask : public JXUrgentTask
{
public:

	LLDBWelcomeTask(const JString& msg, const bool restart);

	virtual ~LLDBWelcomeTask();

	virtual void	Perform() override;

private:

	JString	itsMessage;
	bool	itsRestartFlag;
};

#endif
