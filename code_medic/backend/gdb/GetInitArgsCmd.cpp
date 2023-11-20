/******************************************************************************
 GetInitArgsCmd.cpp

	Gets the initial setting for the arguments to the program set by .gdbinit.
	Unfortunately, we cannot listen for later calls to "set args" because
	one can only define a hook for single word commands.

	BASE CLASS = GetInitArgsCmd

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include "gdb/GetInitArgsCmd.h"
#include "gdb/OutputScanner.h"
#include "globals.h"
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetInitArgsCmd::GetInitArgsCmd
	(
	JXInputField* argInput
	)
	:
	::GetInitArgsCmd(JString("show args", JString::kNoCopy)),
	itsArgInput(argInput)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetInitArgsCmd::~GetInitArgsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

	Use the value from the result.  Otherwise, grab everything after the
	first \" and toss the trailing \"\.

 ******************************************************************************/

static const JRegex resultPattern = "done,value=\"(.*)\"";

void
gdb::GetInitArgsCmd::HandleSuccess
	(
	const JString& data
	)
{
	const JStringMatch m = resultPattern.Match(GetLastResult(), JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
	{
		JString args = m.GetSubstring(1);
		gdb::Output::Scanner::TranslateMIOutput(&args);
		itsArgInput->GetText()->SetText(args);
		return;
	}

	JStringIterator iter(data);
	if (iter.Next("\"") && !iter.AtEnd())
	{
		iter.BeginMatch();
		iter.MoveTo(JStringIterator::kStartAtEnd, 0);
		if (!iter.Prev("\"."))
		{
			iter.MoveTo(JStringIterator::kStartAtEnd, 0);
		}

		JString args = iter.FinishMatch().GetString();
		args.TrimWhitespace();
		itsArgInput->GetText()->SetText(args);
		return;
	}

	Link::Log("GDBGetInitArgsCmd failed to match");
}
