/******************************************************************************
 XDDisplaySourceForMainCmd.cpp

	BASE CLASS = DisplaySourceForMainCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "XDDisplaySourceForMainCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::DisplaySourceForMainCmd::DisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
	:
	::DisplaySourceForMainCmd(sourceDir, JString("status", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::DisplaySourceForMainCmd::~DisplaySourceForMainCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::DisplaySourceForMainCmd::HandleSuccess
	(
	const JString& data
	)
{
}
