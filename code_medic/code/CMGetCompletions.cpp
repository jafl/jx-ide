/******************************************************************************
 CMGetCompletions.cpp

	BASE CLASS = CMCommand

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#include "CMGetCompletions.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetCompletions::CMGetCompletions
	(
	const JString& cmd
	)
	:
	CMCommand(cmd, true, false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetCompletions::~CMGetCompletions()
{
}
