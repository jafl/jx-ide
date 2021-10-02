/******************************************************************************
 GetFullPathCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetFullPathCmd.h"
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* GetFullPathCmd::kFileFound    = "FileFound::GetFullPathCmd";
const JUtf8Byte* GetFullPathCmd::kFileNotFound = "FileNotFound::GetFullPathCmd";
const JUtf8Byte* GetFullPathCmd::kNewCommand   = "NewCommand::GetFullPathCmd";

/******************************************************************************
 Constructor

 ******************************************************************************/

GetFullPathCmd::GetFullPathCmd
	(
	const JString&	cmd,
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	Command(cmd, true, false),
	itsFileName(fileName),
	itsLineIndex(lineIndex)
{
	Command::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetFullPathCmd::~GetFullPathCmd()
{
}
