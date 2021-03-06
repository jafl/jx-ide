/******************************************************************************
 GetFrameCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetFrameCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetFrameCmd::GetFrameCmd
	(
	const JString& cmd
	)
	:
	Command(cmd, false, false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetFrameCmd::~GetFrameCmd()
{
}
