/******************************************************************************
 JVMSetProgramTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMSetProgramTask
#define _H_JVMSetProgramTask

#include <jx-af/jx/JXUrgentTask.h>

class JVMSetProgramTask : public JXUrgentTask
{
public:

	JVMSetProgramTask();

	virtual ~JVMSetProgramTask();

	virtual void	Perform() override;
};

#endif
