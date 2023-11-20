/******************************************************************************
 GetFullPathCmd.cpp

	BASE CLASS = GetFullPathCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/GetFullPathCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetFullPathCmd::GetFullPathCmd
	(
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	::GetFullPathCmd(JString("NOP", JString::kNoCopy), fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetFullPathCmd::~GetFullPathCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetFullPathCmd::HandleSuccess
	(
	const JString& data
	)
{
}
