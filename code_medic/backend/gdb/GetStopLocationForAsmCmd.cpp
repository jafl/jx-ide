/******************************************************************************
 GetStopLocationForAsmCmd.cpp

	BASE CLASS = gdb::GetStopLocationCmd

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "gdb/GetStopLocationForAsmCmd.h"
#include "gdb/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetStopLocationForAsmCmd::GetStopLocationForAsmCmd()
	:
	GetStopLocationCmd()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetStopLocationForAsmCmd::~GetStopLocationForAsmCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
gdb::GetStopLocationForAsmCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	dynamic_cast<Link&>(*GetLink()).SendProgramStopped2(GetLocation());
}
