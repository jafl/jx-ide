/******************************************************************************
 ClearSourceDisplayTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "ClearSourceDisplayTask.h"
#include "SourceDirector.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ClearSourceDisplayTask::ClearSourceDisplayTask
	(
	SourceDirector* dir
	)
{
	itsDirector = dir;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ClearSourceDisplayTask::~ClearSourceDisplayTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
ClearSourceDisplayTask::Perform()
{
	itsDirector->ClearDisplay();
}
