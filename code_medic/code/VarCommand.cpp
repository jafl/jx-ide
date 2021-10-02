/******************************************************************************
 VarCommand.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "VarCommand.h"
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* VarCommand::kValueUpdated = "VarCommand::kValueUpdated";
const JUtf8Byte* VarCommand::kValueFailed  = "VarCommand::kValueFailed";

/******************************************************************************
 Constructor

 ******************************************************************************/

VarCommand::VarCommand()
	:
	Command("", false, true)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

VarCommand::~VarCommand()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
VarCommand::HandleFailure()
{
	Broadcast(ValueMessage(kValueFailed, nullptr));
}
