/******************************************************************************
 DeselectLineTask.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_DeselectLineTask
#define _H_DeselectLineTask

#include <jx-af/jx/JXIdleTask.h>

class LineIndexTable;

class DeselectLineTask : public JXIdleTask
{
public:

	DeselectLineTask(LineIndexTable* table);

	virtual ~DeselectLineTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	LineIndexTable*	itsTable;
};

#endif
