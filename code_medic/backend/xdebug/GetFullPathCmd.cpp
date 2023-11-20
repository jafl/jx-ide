/******************************************************************************
 GetFullPathCmd.cpp

	BASE CLASS = GetFullPathCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/GetFullPathCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetFullPathCmd::GetFullPathCmd
	(
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	::GetFullPathCmd(JString("status", JString::kNoCopy), fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetFullPathCmd::~GetFullPathCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::GetFullPathCmd::HandleSuccess
	(
	const JString& data
	)
{
}
