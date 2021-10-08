/******************************************************************************
 LLDBSymbolsLoadedTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBSymbolsLoadedTask.h"
#include "LLDBLink.h"
#include "globals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::SymbolsLoadedTask::SymbolsLoadedTask
	(
	const JString& fileName
	)
	:
	itsFileName(fileName)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::SymbolsLoadedTask::~SymbolsLoadedTask()
{
}

/******************************************************************************
 Perform

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
