/******************************************************************************
 GetInitArgs.cpp

	Gets the initial setting for the arguments to the program set by .gdbinit.
	Unfortunately, we cannot listen for later calls to "set args" because
	one can only define a hook for single word commands.

	BASE CLASS = Command

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include "GetInitArgs.h"
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetInitArgs::GetInitArgs
	(
	const JString& cmd
	)
	:
	Command(cmd, true, false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetInitArgs::~GetInitArgs()
{
}
