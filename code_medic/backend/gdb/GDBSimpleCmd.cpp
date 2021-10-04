/******************************************************************************
 GDBSimpleCmd.cpp

	Sends a command that does not expect any response.

	BASE CLASS = Command

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "GDBSimpleCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::SimpleCmd::SimpleCmd
	(
	const JString& cmd
	)
	:
	Command(cmd, true, false)
{
	ShouldIgnoreResult(true);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::SimpleCmd::~SimpleCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
gdb::SimpleCmd::HandleSuccess
	(
	const JString& data
	)
{
}
