/******************************************************************************
 XDGetCompletions.cpp

	BASE CLASS = GetCompletionsCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetCompletions.h"
#include "CommandInput.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetCompletions::XDGetCompletions
	(
	CommandInput*			input,
	CommandOutputDisplay*	history
	)
	:
	GetCompletionsCmd(JString("status", JString::kNoCopy)),
	itsPrefix(input->GetText()->GetText()),
	itsInput(input),
	itsHistory(history)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetCompletions::~XDGetCompletions()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetCompletions::HandleSuccess
	(
	const JString& data
	)
{
}
