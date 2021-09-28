/******************************************************************************
 JVMGetCompletions.cpp

	BASE CLASS = CMGetCompletions

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetCompletions.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetCompletions::JVMGetCompletions
	(
	CMCommandInput*			input,
	CMCommandOutputDisplay*	history
	)
	:
	CMGetCompletions(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetCompletions::~JVMGetCompletions()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetCompletions::HandleSuccess
	(
	const JString& data
	)
{
}
