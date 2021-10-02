/******************************************************************************
 GDBSimpleCommand.cpp

	Sends a command that does not expect any response.

	BASE CLASS = Command

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "GDBSimpleCommand.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBSimpleCommand::GDBSimpleCommand
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

GDBSimpleCommand::~GDBSimpleCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBSimpleCommand::HandleSuccess
	(
	const JString& data
	)
{
}
