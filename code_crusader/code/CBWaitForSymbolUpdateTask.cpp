/******************************************************************************
 CBWaitForSymbolUpdateTask.cpp

	We cannot do this inside CBProjectDocument::SymbolUpdateProgress()
	because the ACE reactor is not re-entrant.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#include "CBWaitForSymbolUpdateTask.h"
#include <jx-af/jx/JXApplication.h>
#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBWaitForSymbolUpdateTask::CBWaitForSymbolUpdateTask
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

CBWaitForSymbolUpdateTask::~CBWaitForSymbolUpdateTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CBWaitForSymbolUpdateTask::Perform()
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
