/******************************************************************************
 VarCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "VarCmd.h"
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* VarCmd::kValueUpdated = "Updated::VarCmd";
const JUtf8Byte* VarCmd::kValueFailed  = "Failed::VarCmd";

/******************************************************************************
 Constructor

 ******************************************************************************/

VarCmd::VarCmd()
	:
	Command("", false, true)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

VarCmd::~VarCmd()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
VarCmd::HandleFailure()
{
	Broadcast(ValueMessage(kValueFailed, nullptr));
}
