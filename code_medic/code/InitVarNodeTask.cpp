/******************************************************************************
 InitVarNodeTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "InitVarNodeTask.h"
#include "VarNode.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

InitVarNodeTask::InitVarNodeTask
	(
	VarNode* node
	)
{
	itsNode = node;
	ClearWhenGoingAway(itsNode, &itsNode);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

InitVarNodeTask::~InitVarNodeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
InitVarNodeTask::Perform()
{
	if (itsNode != nullptr)
	{
		itsNode->NameChanged();
	}
}
