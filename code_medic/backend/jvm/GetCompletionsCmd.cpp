/******************************************************************************
 GetCompletionsCmd.cpp

	BASE CLASS = GetCompletionsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/GetCompletionsCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetCompletionsCmd::GetCompletionsCmd
	(
	CommandInput*			input,
	CommandOutputDisplay*	history
	)
	:
	::GetCompletionsCmd(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetCompletionsCmd::~GetCompletionsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetCompletionsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
