/******************************************************************************
 LLDBGetInitArgsCmd.cpp

	Gets the initial setting for the arguments to the program set by .lldbinit

	BASE CLASS = GetInitArgsCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetInitArgsCmd.h"
#include "globals.h"
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetInitArgsCmd::GetInitArgsCmd
	(
	JXInputField* argInput
	)
	:
	::GetInitArgsCmd(JString::empty),
	itsArgInput(argInput)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetInitArgsCmd::~GetInitArgsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
lldb::GetInitArgsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
