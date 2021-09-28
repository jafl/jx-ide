/******************************************************************************
 CMUpdateCommandsTableWidth.h

	Interface for the CMUpdateCommandsTableWidth class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMUpdateCommandsTableWidth
#define _H_CMUpdateCommandsTableWidth

#include <jx-af/jx/JXUrgentTask.h>

class CMEditCommandsTable;

class CMUpdateCommandsTableWidth : public JXUrgentTask
{
public:

	CMUpdateCommandsTableWidth(CMEditCommandsTable* table);

	virtual ~CMUpdateCommandsTableWidth();

	virtual void	Perform();

private:

	CMEditCommandsTable*	itsTable;		// we don't own this
};

#endif
