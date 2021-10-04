/******************************************************************************
 JVMDisplaySourceForMainCmd.cpp

	BASE CLASS = DisplaySourceForMainCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMDisplaySourceForMainCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::DisplaySourceForMainCmd::DisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
	:
	::DisplaySourceForMainCmd(sourceDir, JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::DisplaySourceForMainCmd::~DisplaySourceForMainCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::DisplaySourceForMainCmd::HandleSuccess
	(
	const JString& data
	)
{
}
