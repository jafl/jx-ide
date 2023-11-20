/******************************************************************************
 GetFullPathCmd.cpp

	BASE CLASS = GetFullPathCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/GetFullPathCmd.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetFullPathCmd::GetFullPathCmd
	(
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	::GetFullPathCmd(JString::empty, fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetFullPathCmd::~GetFullPathCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
lldb::GetFullPathCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	Broadcast(FileNotFound(GetFileName()));
	GetLink()->RememberFile(GetFileName(), JString::empty);
}
