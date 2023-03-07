/******************************************************************************
 InitVarNodeTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "InitVarNodeTask.h"
#include "VarNode.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

InitVarNodeTask::InitVarNodeTask
	(
	VarNode* node
	)
	:
	JXUrgentTask(node),
	itsNode(node)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

InitVarNodeTask::~InitVarNodeTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
InitVarNodeTask::Perform()
{
	itsNode->NameChanged();
}
