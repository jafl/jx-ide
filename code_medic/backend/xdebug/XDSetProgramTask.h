/******************************************************************************
 XDSetProgramTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDSetProgramTask
#define _H_XDSetProgramTask

#include <jx-af/jx/JXUrgentTask.h>

class XDSetProgramTask : public JXUrgentTask
{
public:

	XDSetProgramTask();

	virtual ~XDSetProgramTask();

	virtual void	Perform() override;
};

#endif
