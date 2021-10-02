/******************************************************************************
 UpdateCommandsTableWidth.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "UpdateCommandsTableWidth.h"
#include "EditCommandsTable.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UpdateCommandsTableWidth::UpdateCommandsTableWidth
	(
	EditCommandsTable* table
	)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UpdateCommandsTableWidth::~UpdateCommandsTableWidth()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
UpdateCommandsTableWidth::Perform()
{
	itsTable->AdjustColWidth();
}
