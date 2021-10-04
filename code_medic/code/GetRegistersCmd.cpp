/******************************************************************************
 GetRegistersCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GetRegistersCmd.h"
#include "RegistersDir.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetRegistersCmd::GetRegistersCmd
	(
	const JString&	cmd,
	RegistersDir*	dir
	)
	:
	Command(cmd, false, true),
	itsDir(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetRegistersCmd::~GetRegistersCmd()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetRegistersCmd::HandleFailure()
{
	itsDir->Update(JString::empty);
}
