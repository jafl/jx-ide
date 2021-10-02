/******************************************************************************
 AdjustLineTableToTextTask.cpp

	AdjustToText() cannot be called by LineIndexTable ctor.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "AdjustLineTableToTextTask.h"
#include "LineIndexTable.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

AdjustLineTableToTextTask::AdjustLineTableToTextTask
	(
	LineIndexTable* table
	)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

AdjustLineTableToTextTask::~AdjustLineTableToTextTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
AdjustLineTableToTextTask::Perform()
{
	itsTable->AdjustToText();
}
