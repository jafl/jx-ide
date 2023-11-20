/******************************************************************************
 SymbolsLoadedTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/SymbolsLoadedTask.h"
#include "lldb/Link.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::SymbolsLoadedTask::SymbolsLoadedTask
	(
	const JString& fileName
	)
	:
	JXUrgentTask(GetLink()),
	itsFileName(fileName)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

lldb::SymbolsLoadedTask::~SymbolsLoadedTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
lldb::SymbolsLoadedTask::Perform()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link != nullptr)
	{
		link->SymbolsLoaded(itsFileName);
	}
}
