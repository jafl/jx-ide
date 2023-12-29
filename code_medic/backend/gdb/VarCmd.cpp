/******************************************************************************
 VarCmd.cpp

	BASE CLASS = VarCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "gdb/VarCmd.h"
#include "gdb/VarTreeParser.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::VarCmd::VarCmd
	(
	const JString& origCmd
	)
	:
	::VarCmd()
{
	JString cmd("set print pretty off\nset print array off\n"
				"set print repeats 0\nset width 0\n");
	cmd        += origCmd;
	SetCommand(cmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::VarCmd::~VarCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static JRegex prefixPattern(
	"^([^\v]*\v\n*|warning:[^\n\v]*[\n\v]+)*[[:space:]]*"
	"\\$[[:digit:]]+[[:space:]]*=[[:space:]]*");

void
gdb::VarCmd::HandleSuccess
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

		VarTreeParser parser;
		if (parser.Parse(s) == 0)
		{
			parser.ReportRecoverableError();
			success = true;
			Broadcast(ValueMessage(kValueUpdated, parser.GetRootNode()));
		}
	}
	else
	{
		Link::Log("gdb::VarCmd failed to match");
	}

	if (!success)
	{
		Broadcast(ValueMessage(kValueFailed, nullptr));
	}

	s.Clear();
	SetData(s);
}
