/******************************************************************************
 GDBVarCommand.cpp

	BASE CLASS = VarCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBVarCommand.h"
#include "GDBVarTreeParser.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBVarCommand::GDBVarCommand
	(
	const JString& origCmd
	)
	:
	VarCommand()
{
	JString cmd("set print pretty off\nset print array off\n"
				"set print repeats 0\nset width 0\n");
	cmd        += origCmd;
	SetCommand(cmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBVarCommand::~GDBVarCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static JRegex prefixPattern =
	"^([^\v]*\v\n*|warning:[^\n\v]*[\n\v]+)*[[:space:]]*"
	"\\$[[:digit:]]+[[:space:]]*=[[:space:]]*";

void
GDBVarCommand::HandleSuccess
	(
	const JString& data
	)
{
	JString s = data;
	s.TrimWhitespace();

	bool success = false;

	prefixPattern.SetSingleLine();

	JStringIterator iter(&s);
	if (iter.Next(prefixPattern))
	{
		iter.RemoveAllPrev();
		iter.Invalidate();
		SetData(s);

		GDBVarTreeParser parser;
		if (parser.Parse(s) == 0)
		{
			parser.ReportRecoverableError();
			success = true;
			Broadcast(ValueMessage(kValueUpdated, parser.GetRootNode()));
		}
	}
	else
	{
		GetLink()->Log("GDBVarCommand failed to match");
	}

	if (!success)
	{
		Broadcast(ValueMessage(kValueFailed, nullptr));
	}

	s.Clear();
	SetData(s);
}
