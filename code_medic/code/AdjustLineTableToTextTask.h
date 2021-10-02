/******************************************************************************
 AdjustLineTableToTextTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_AdjustLineTableToTextTask
#define _H_AdjustLineTableToTextTask

#include <jx-af/jx/JXUrgentTask.h>

class LineIndexTable;

class AdjustLineTableToTextTask : public JXUrgentTask
{
public:

	AdjustLineTableToTextTask(LineIndexTable* table);

	virtual ~AdjustLineTableToTextTask();

	virtual void	Perform();

private:

	LineIndexTable*	itsTable;	// not owned

private:

	// not allowed

	AdjustLineTableToTextTask(const AdjustLineTableToTextTask& source);
	const AdjustLineTableToTextTask& operator=(const AdjustLineTableToTextTask& source);
};

#endif
