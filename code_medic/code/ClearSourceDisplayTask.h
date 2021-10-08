/******************************************************************************
 ClearSourceDisplayTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_ClearSourceDisplayTask
#define _H_ClearSourceDisplayTask

#include <jx-af/jx/JXUrgentTask.h>

class SourceDirector;

class ClearSourceDisplayTask : public JXUrgentTask
{
public:

	ClearSourceDisplayTask(SourceDirector* dir);

	~ClearSourceDisplayTask() override;

	void	Perform() override;

private:

	SourceDirector*	itsDirector;	// not owned
};

#endif
