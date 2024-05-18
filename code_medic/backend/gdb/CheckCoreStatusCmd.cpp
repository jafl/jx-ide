/******************************************************************************
 CheckCoreStatusCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "gdb/CheckCoreStatusCmd.h"
#include "gdb/Link.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::CheckCoreStatusCmd::CheckCoreStatusCmd()
	:
	Command("info files", true, false)
{
	Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::CheckCoreStatusCmd::~CheckCoreStatusCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex coreFilePattern("Local core dump file:\n\t`([^\n]+)'");

void
gdb::CheckCoreStatusCmd::HandleSuccess
	(
	const JString& data
	)
{
	JString fileName;

	const JStringMatch m = coreFilePattern.Match(data, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
	{
		fileName = m.GetSubstring(1);
	}
	else
	{
		Link::Log("GDBCheckCoreStatusCmd failed to match (ok if there is no core file)");
	}

	// Link has to broadcast status of core regardless of whether or not
	// we get what we expect from gdb.

	dynamic_cast<Link&>(*GetLink()).SaveCoreName(fileName);
}
