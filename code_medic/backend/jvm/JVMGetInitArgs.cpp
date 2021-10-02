/******************************************************************************
 JVMGetInitArgs.cpp

	BASE CLASS = GetInitArgs

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetInitArgs.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetInitArgs::JVMGetInitArgs
	(
	JXInputField* argInput
	)
	:
	GetInitArgs(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetInitArgs::~JVMGetInitArgs()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetInitArgs::HandleSuccess
	(
	const JString& data
	)
{
}
