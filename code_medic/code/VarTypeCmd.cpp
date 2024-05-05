/******************************************************************************
 VarTypeCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#include "VarTypeCmd.h"
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* VarTypeCmd::kTypeInfo = "TypeInfo::VarTypeCmd";

/******************************************************************************
 Constructor

 ******************************************************************************/

VarTypeCmd::VarTypeCmd()
	:
	Command("", false, true)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

VarTypeCmd::~VarTypeCmd()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
VarTypeCmd::HandleFailure()
{
}
