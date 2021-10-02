/******************************************************************************
 InitVarNodeTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_InitVarNodeTask
#define _H_InitVarNodeTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JBroadcaster.h>

class VarNode;

class InitVarNodeTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	InitVarNodeTask(VarNode* node);

	virtual ~InitVarNodeTask();

	virtual void	Perform();

private:

	VarNode*	itsNode;		// we don't own this
};

#endif
