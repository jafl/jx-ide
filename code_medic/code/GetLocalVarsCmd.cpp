/******************************************************************************
 GetLocalVarsCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetLocalVarsCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetLocalVarsCmd::GetLocalVarsCmd
	(
	const JString& cmd
	)
	:
	Command(cmd, false, true)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetLocalVarsCmd::~GetLocalVarsCmd()
{
}