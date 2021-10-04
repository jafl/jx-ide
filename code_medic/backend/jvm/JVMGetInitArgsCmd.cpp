/******************************************************************************
 JVMGetInitArgsCmd.cpp

	BASE CLASS = GetInitArgsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetInitArgsCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetInitArgsCmd::GetInitArgsCmd
	(
	JXInputField* argInput
	)
	:
	::GetInitArgsCmd(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetInitArgsCmd::~GetInitArgsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetInitArgsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
