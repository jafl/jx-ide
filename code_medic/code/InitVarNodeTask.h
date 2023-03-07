/******************************************************************************
 InitVarNodeTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_InitVarNodeTask
#define _H_InitVarNodeTask

#include <jx-af/jx/JXUrgentTask.h>

class VarNode;

class InitVarNodeTask : public JXUrgentTask
{
public:

	InitVarNodeTask(VarNode* node);

protected:

	~InitVarNodeTask() override;

	void	Perform() override;

private:

	VarNode*	itsNode;		// we don't own this
};

#endif
