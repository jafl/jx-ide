/******************************************************************************
 GDBGetProgramNameCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetProgramNameCmd.h"
#include "GDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetProgramNameCmd::GetProgramNameCmd()
	:
	Command("info files", true, false)
{
	Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetProgramNameCmd::~GetProgramNameCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex fileNamePattern = "Local exec file:\n\t`([^\n]+)'";

void
gdb::GetProgramNameCmd::HandleSuccess
	(
	const JString& data
	)
{
	JString fileName;

	const JStringMatch m = fileNamePattern.Match(data, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
	{
		fileName = m.GetSubstring(1);
	}
	else
	{
		Link::Log("GDBGetProgramNameCmd failed to match");
	}

	// Link has to broadcast SymbolsLoaded regardless of whether or not
	// we get what we expect from gdb.

	dynamic_cast<Link*>(GetLink())->SaveProgramName(fileName);
}
