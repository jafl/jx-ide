/******************************************************************************
 GetInitArgsCmd.cpp

	Gets the initial setting for the arguments to the program set by .gdbinit.
	Unfortunately, we cannot listen for later calls to "set args" because
	one can only define a hook for single word commands.

	BASE CLASS = GetInitArgsCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/GetInitArgsCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetInitArgsCmd::GetInitArgsCmd
	(
	JXInputField* argInput
	)
	:
	::GetInitArgsCmd(JString("status", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetInitArgsCmd::~GetInitArgsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::GetInitArgsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
