/******************************************************************************
 JVMSetProgramTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMSetProgramTask
#define _H_JVMSetProgramTask

#include <jx-af/jx/JXUrgentTask.h>

namespace jvm {

class SetProgramTask : public JXUrgentTask
{
public:

	SetProgramTask();

	~SetProgramTask() override;

	void	Perform() override;
};

};

#endif
