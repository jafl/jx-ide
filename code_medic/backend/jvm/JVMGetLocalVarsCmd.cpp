/******************************************************************************
 JVMGetLocalVarsCmd.cpp

	BASE CLASS = GetLocalVarsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetLocalVarsCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetLocalVarsCmd::JVMGetLocalVarsCmd
	(
	VarNode* rootNode
	)
	:
	GetLocalVarsCmd(JString("NOP", JString::kNoCopy)),
	itsRootNode(rootNode)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetLocalVarsCmd::~JVMGetLocalVarsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetLocalVarsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
