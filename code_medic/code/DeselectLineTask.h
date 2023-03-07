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

	~DeselectLineTask() override;

protected:

	void	Perform(const Time delta) override;

private:

	LineIndexTable*	itsTable;
};

#endif
