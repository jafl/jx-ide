/******************************************************************************
 VarTypeCmd.cpp

	BASE CLASS = VarTypeCmd

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#include "gdb/VarTypeCmd.h"
#include "gdb/VarTreeParser.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::VarTypeCmd::VarTypeCmd
	(
	const JString& origCmd
	)
	:
	::VarTypeCmd()
{
	JString cmd("set print object on\n");
	cmd        += origCmd;
	SetCommand(cmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::VarTypeCmd::~VarTypeCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static JRegex typePattern("/\\* real type = (.+?) \\*/");

void
gdb::VarTypeCmd::HandleSuccess
	(
	const JString& data
	)
{
	JString s = data;
	s.TrimWhitespace();

	JStringIterator iter(s);
	if (iter.Next(typePattern))
	{
		const JStringMatch& m = iter.GetLastMatch();
		SetData(m.GetSubstring(1));
		Broadcast(TypeInfo(GetData()));
	}
	else
	{
		Link::Log("gdb::VarTypeCmd failed to match");
	}

	s.Clear();
	SetData(s);
}
