/******************************************************************************
 CommandOutputPostFTCTask.cpp

	This creates additional widgets in CommandOutputDocument *after* FTC, to
	keep everything aligned.

	BASE CLASS = JXUrgentTask

	Copyright © 2017 by John Lindal.

 ******************************************************************************/

#include "CommandOutputPostFTCTask.h"
#include "CommandOutputDocument.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

CommandOutputPostFTCTask::CommandOutputPostFTCTask
	(
	CommandOutputDocument* doc
	)
	:
	JXUrgentTask(doc),
	itsDoc(doc)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

CommandOutputPostFTCTask::~CommandOutputPostFTCTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
CommandOutputPostFTCTask::Perform()
{
	itsDoc->PlaceCustomWidgets();
}
