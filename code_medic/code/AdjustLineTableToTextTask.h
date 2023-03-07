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

protected:

	~AdjustLineTableToTextTask() override;

	void	Perform() override;

private:

	LineIndexTable*	itsTable;	// not owned
};

#endif
