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

JVMGetInitArgsCmd::JVMGetInitArgsCmd
	(
	JXInputField* argInput
	)
	:
	GetInitArgsCmd(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetInitArgsCmd::~JVMGetInitArgsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetInitArgsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
