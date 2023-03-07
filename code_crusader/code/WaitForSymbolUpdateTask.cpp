/******************************************************************************
 WaitForSymbolUpdateTask.cpp

	We cannot do this inside ProjectDocument::SymbolUpdateProgress()
	because the ACE reactor is not re-entrant.

	BASE CLASS = JXUrgentTask

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#include "WaitForSymbolUpdateTask.h"
#include <jx-af/jcore/JThisProcess.h>
#include <jx-af/jcore/jTime.h>
#include <boost/fiber/operations.hpp>
#include <chrono>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WaitForSymbolUpdateTask::WaitForSymbolUpdateTask
	(
	JProcess* p
	)
	:
	JXUrgentTask(p),
	itsProcess(p),
	itsKeepWaitingFlag(true)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

WaitForSymbolUpdateTask::~WaitForSymbolUpdateTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
WaitForSymbolUpdateTask::Perform()
{
	const auto start = time(nullptr);
	const auto delta = std::chrono::duration<long, std::deci>(1);

	while (itsKeepWaitingFlag && itsProcess != nullptr)
	{
		JThisProcess::CheckACEReactor();
		JProcess::CheckForFinishedChild(false);
		boost::this_fiber::sleep_for(delta);

		if (time(nullptr) - start > 30)
		{
			itsProcess->Kill();
			break;
		}
	}
}
