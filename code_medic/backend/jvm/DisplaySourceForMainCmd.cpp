/******************************************************************************
 DisplaySourceForMainCmd.cpp

	BASE CLASS = DisplaySourceForMainCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/DisplaySourceForMainCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::DisplaySourceForMainCmd::DisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
	:
	::DisplaySourceForMainCmd(sourceDir, "NOP")
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
