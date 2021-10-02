/******************************************************************************
 JVMVarCommand.cpp

	BASE CLASS = VarCommand

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMVarCommand.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMVarCommand::JVMVarCommand
	(
	const JString& origCmd
	)
	:
	VarCommand()
{
	SetCommand(JString("NOP", JString::kNoCopy));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMVarCommand::~JVMVarCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMVarCommand::HandleSuccess
	(
	const JString& data
	)
{
}
