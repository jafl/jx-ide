/******************************************************************************
 XDSetProgramTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDSetProgramTask
#define _H_XDSetProgramTask

#include <jx-af/jx/JXUrgentTask.h>

namespace xdebug {

class SetProgramTask : public JXUrgentTask
{
public:

	SetProgramTask();

protected:

	~SetProgramTask() override;

	void	Perform() override;
};

};

#endif
