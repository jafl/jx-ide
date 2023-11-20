/******************************************************************************
 PingTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBPingTask
#define _H_GDBPingTask

#include <jx-af/jx/JXIdleTask.h>

namespace gdb {

class PingTask : public JXIdleTask
{
public:

	PingTask();

	~PingTask() override;

protected:

	void	Perform(const Time delta) override;
};

};

#endif
