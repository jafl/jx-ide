/******************************************************************************
 EventTask.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBEventTask
#define _H_LLDBEventTask

#include <jx-af/jx/JXIdleTask.h>

namespace lldb {

class Link;

class EventTask : public JXIdleTask
{
public:

	EventTask(Link* link);

	~EventTask() override;

protected:

	void	Perform(const Time delta) override;

private:

	Link*	itsLink;
};

};

#endif
