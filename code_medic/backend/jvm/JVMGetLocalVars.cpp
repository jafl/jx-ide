/******************************************************************************
 JVMGetLocalVars.cpp

	BASE CLASS = GetLocalVars

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetLocalVars.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetLocalVars::JVMGetLocalVars
	(
	VarNode* rootNode
	)
	:
	GetLocalVars(JString("NOP", JString::kNoCopy)),
	itsRootNode(rootNode)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetLocalVars::~JVMGetLocalVars()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetLocalVars::HandleSuccess
	(
	const JString& data
	)
{
}
