/******************************************************************************
 GetCompletionsCmd.cpp

	BASE CLASS = GetCompletionsCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/GetCompletionsCmd.h"
#include "CommandInput.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetCompletionsCmd::GetCompletionsCmd
	(
	CommandInput*			input,
	CommandOutputDisplay*	history
	)
	:
	::GetCompletionsCmd(JString("status", JString::kNoCopy)),
	itsPrefix(input->GetText()->GetText()),
	itsInput(input),
	itsHistory(history)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetCompletionsCmd::~GetCompletionsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::GetCompletionsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
