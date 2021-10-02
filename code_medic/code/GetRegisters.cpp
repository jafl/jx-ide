/******************************************************************************
 GetRegisters.cpp

	BASE CLASS = Command

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GetRegisters.h"
#include "RegistersDir.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetRegisters::GetRegisters
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

GetRegisters::~GetRegisters()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetRegisters::HandleFailure()
{
	itsDir->Update(JString::empty);
}
