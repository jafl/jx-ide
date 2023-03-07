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
	:
	JXUrgentTask(table),
	itsTable(table)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

UpdateCommandsTableWidth::~UpdateCommandsTableWidth()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
UpdateCommandsTableWidth::Perform()
{
	itsTable->AdjustColWidth();
}
