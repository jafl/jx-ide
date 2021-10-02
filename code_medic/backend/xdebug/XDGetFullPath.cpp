/******************************************************************************
 XDGetFullPath.cpp

	BASE CLASS = GetFullPathCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetFullPath.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetFullPath::XDGetFullPath
	(
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	GetFullPathCmd(JString("status", JString::kNoCopy), fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetFullPath::~XDGetFullPath()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetFullPath::HandleSuccess
	(
	const JString& data
	)
{
}
