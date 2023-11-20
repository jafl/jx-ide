/******************************************************************************
 GetCompletionsCmd.cpp

	BASE CLASS = GetCompletionsCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/GetCompletionsCmd.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBStringList.h"
#include "CommandInput.h"
#include "CommandOutputDisplay.h"
#include "globals.h"
#include "lldb/Link.h"	// must be after X11 includes: Status
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetCompletionsCmd::GetCompletionsCmd
	(
	CommandInput*			input,
	CommandOutputDisplay*	history
	)
	:
	::GetCompletionsCmd(JString::empty),
	itsPrefix(input->GetText()->GetText()),
	itsInput(input),
	itsHistory(history)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetCompletionsCmd::~GetCompletionsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
lldb::GetCompletionsCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	SBCommandInterpreter interp = link->GetDebugger()->GetCommandInterpreter();
	if (!interp.IsValid())
	{
		return;
	}

	SBStringList matches;
	const JSize matchCount = interp.HandleCompletion(itsPrefix.GetBytes(), itsPrefix.GetByteCount(), 0, -1, matches);

	if (matchCount == 0)
	{
		itsInput->GetDisplay()->Beep();
		return;
	}
	else if (strlen(matches.GetStringAtIndex(0)) > 0)
	{
		itsInput->GoToEndOfLine();
		itsInput->Paste(JString(matches.GetStringAtIndex(0), JString::kNoCopy));
		return;
	}
	else if (matchCount == 1)
	{
		itsInput->GetText()->SetText(JString(matches.GetStringAtIndex(1), JString::kNoCopy));
		itsInput->GoToEndOfLine();
		return;
	}

	itsHistory->PlaceCursorAtEnd();
	itsHistory->Paste(JString::newline);

	const JSize count = matches.GetSize();
	for (JIndex i=1; i<count; i++)
	{
		itsHistory->Paste(JString(matches.GetStringAtIndex(i), JString::kNoCopy));
		itsHistory->Paste(JString::newline);
	}
}
