/******************************************************************************
 JVMGetCompletionsCmd.cpp

	BASE CLASS = GetCompletionsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetCompletionsCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetCompletionsCmd::JVMGetCompletionsCmd
	(
	CommandInput*			input,
	CommandOutputDisplay*	history
	)
	:
	GetCompletionsCmd(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetCompletionsCmd::~JVMGetCompletionsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetCompletionsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
