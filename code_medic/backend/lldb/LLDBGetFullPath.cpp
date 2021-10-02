/******************************************************************************
 LLDBGetFullPath.cpp

	BASE CLASS = GetFullPathCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetFullPath.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetFullPath::LLDBGetFullPath
	(
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	GetFullPathCmd(JString::empty, fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetFullPath::~LLDBGetFullPath()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetFullPath::HandleSuccess
	(
	const JString& cmdData
	)
{
	Broadcast(FileNotFound(GetFileName()));
	GetLink()->RememberFile(GetFileName(), JString::empty);
}
