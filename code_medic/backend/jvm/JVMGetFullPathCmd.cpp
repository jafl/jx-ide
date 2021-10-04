/******************************************************************************
 JVMGetFullPathCmd.cpp

	BASE CLASS = GetFullPathCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetFullPathCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetFullPathCmd::JVMGetFullPathCmd
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

JVMGetFullPathCmd::~JVMGetFullPathCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetFullPathCmd::HandleSuccess
	(
	const JString& data
	)
{
}
