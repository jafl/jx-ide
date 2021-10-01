/******************************************************************************
 ExecOutputPostFTCTask.cpp

	This creates additional widgets in ExecOutputDocument *after* FTC, to
	keep everything aligned.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 2017 by John Lindal.

 ******************************************************************************/

#include "ExecOutputPostFTCTask.h"
#include "ExecOutputDocument.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ExecOutputPostFTCTask::ExecOutputPostFTCTask
	(
	ExecOutputDocument* doc
	)
	:
	itsDoc(doc)
{
	ClearWhenGoingAway(itsDoc, &itsDoc);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ExecOutputPostFTCTask::~ExecOutputPostFTCTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
ExecOutputPostFTCTask::Perform()
{
	if (itsDoc != nullptr)
	{
		itsDoc->PlaceCmdLineWidgets();
	}
}
