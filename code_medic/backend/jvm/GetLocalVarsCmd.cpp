/******************************************************************************
 GetLocalVarsCmd.cpp

	BASE CLASS = GetLocalVarsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/GetLocalVarsCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetLocalVarsCmd::GetLocalVarsCmd
	(
	VarNode* rootNode
	)
	:
	::GetLocalVarsCmd("NOP"),
	itsRootNode(rootNode)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetLocalVarsCmd::~GetLocalVarsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetLocalVarsCmd::HandleSuccess
	(
	const JString& data
	)
{
}
