/******************************************************************************
 DeselectLineTask.cpp

	Deselects the line number after the Line menu is closed.

	BASE CLASS = JXIdleTask

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "DeselectLineTask.h"
#include "LineIndexTable.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

const Time kCheckInterval = 500;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

DeselectLineTask::DeselectLineTask
	(
	LineIndexTable* table
	)
	:
	JXIdleTask(kCheckInterval),
	itsTable(table)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DeselectLineTask::~DeselectLineTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
DeselectLineTask::Perform
	(
	const Time delta
	)
{
	if (!itsTable->itsLineMenu->IsOpen())
	{
		itsTable->GetTableSelection().ClearSelection();
		itsTable->itsDeselectTask = nullptr;
		jdelete this;
	}
}
