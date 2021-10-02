/******************************************************************************
 GetCompletionsCmd.cpp

	BASE CLASS = Command

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#include "GetCompletionsCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetCompletionsCmd::GetCompletionsCmd
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

GetCompletionsCmd::~GetCompletionsCmd()
{
}
