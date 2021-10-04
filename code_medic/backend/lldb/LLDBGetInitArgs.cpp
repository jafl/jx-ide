/******************************************************************************
 LLDBGetInitArgs.cpp

	Gets the initial setting for the arguments to the program set by .lldbinit

	BASE CLASS = GetInitArgsCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetInitArgs.h"
#include "globals.h"
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetInitArgs::LLDBGetInitArgs
	(
	JXInputField* argInput
	)
	:
	GetInitArgsCmd(JString::empty),
	itsArgInput(argInput)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetInitArgs::~LLDBGetInitArgs()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetInitArgs::HandleSuccess
	(
	const JString& data
	)
{
}
