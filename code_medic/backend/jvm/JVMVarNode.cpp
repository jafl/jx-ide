/******************************************************************************
 JVMVarNode.cpp

	BASE CLASS = public VarNode

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#include "JVMVarNode.h"
#include "VarCommand.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JVMVarNode::JVMVarNode				// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	VarNode(shouldUpdate)
{
}

JVMVarNode::JVMVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
	:
	VarNode(parent, name, value)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JVMVarNode::~JVMVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
JVMVarNode::GetFullName
	(
	bool* isPointer
	)
	const
{
	return GetFullNameForCFamilyLanguage(isPointer);
}
