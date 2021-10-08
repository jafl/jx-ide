/******************************************************************************
 UpdateCommandsTableWidth.h

	Interface for the UpdateCommandsTableWidth class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_UpdateCommandsTableWidth
#define _H_UpdateCommandsTableWidth

#include <jx-af/jx/JXUrgentTask.h>

class EditCommandsTable;

class UpdateCommandsTableWidth : public JXUrgentTask
{
public:

	UpdateCommandsTableWidth(EditCommandsTable* table);

	~UpdateCommandsTableWidth() override;

	void	Perform() override;

private:

	EditCommandsTable*	itsTable;		// we don't own this
};

#endif
