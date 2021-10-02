/******************************************************************************
 JVMGetFullPath.cpp

	BASE CLASS = GetFullPathCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetFullPath.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetFullPath::JVMGetFullPath
	(
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	GetFullPathCmd(JString("NOP", JString::kNoCopy), fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetFullPath::~JVMGetFullPath()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetFullPath::HandleSuccess
	(
	const JString& data
	)
{
}
