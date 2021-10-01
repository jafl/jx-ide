/******************************************************************************
 WaitForSymbolUpdateTask.cpp

	We cannot do this inside ProjectDocument::SymbolUpdateProgress()
	because the ACE reactor is not re-entrant.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#include "WaitForSymbolUpdateTask.h"
#include <jx-af/jx/JXApplication.h>
#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WaitForSymbolUpdateTask::WaitForSymbolUpdateTask
	(
	JProcess* p
	)
	:
	itsProcess(p)
{
	itsKeepWaitingFlag = true;
	ClearWhenGoingAway(itsProcess, &itsProcess);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WaitForSymbolUpdateTask::~WaitForSymbolUpdateTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
WaitForSymbolUpdateTask::Perform()
{
	const time_t start = time(nullptr);
	while (itsKeepWaitingFlag && itsProcess != nullptr)
	{
		if (time(nullptr) - start > 30)
		{
			itsProcess->Kill();
			break;
		}

		JWait(0.1);
		JXApplication::CheckACEReactor();
		JProcess::CheckForFinishedChild(false);
	}
}
