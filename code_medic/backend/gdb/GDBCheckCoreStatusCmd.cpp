/******************************************************************************
 GDBCheckCoreStatusCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBCheckCoreStatusCmd.h"
#include "GDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBCheckCoreStatusCmd::GDBCheckCoreStatusCmd()
	:
	Command("info files", true, false)
{
	Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBCheckCoreStatusCmd::~GDBCheckCoreStatusCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex coreFilePattern = "Local core dump file:\n\t`([^\n]+)'";

void
GDBCheckCoreStatusCmd::HandleSuccess
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
		GetLink()->Log("GDBCheckCoreStatusCmd failed to match (ok if there is no core file)");
	}

	// Link has to broadcast status of core regardless of whether or not
	// we get what we expect from gdb.

	dynamic_cast<GDBLink*>(GetLink())->SaveCoreName(fileName);
}
