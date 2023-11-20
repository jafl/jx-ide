/******************************************************************************
 GetStopLocationForLinkCmd.cpp

	BASE CLASS = gdb::GetStopLocationCmd

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "gdb/GetStopLocationForLinkCmd.h"
#include "gdb/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetStopLocationForLinkCmd::GetStopLocationForLinkCmd()
	:
	gdb::GetStopLocationCmd()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetStopLocationForLinkCmd::~GetStopLocationForLinkCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
gdb::GetStopLocationForLinkCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	dynamic_cast<Link*>(GetLink())->SendProgramStopped(GetLocation());
}
