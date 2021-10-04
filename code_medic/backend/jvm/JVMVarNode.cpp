/******************************************************************************
 JVMVarNode.cpp

	BASE CLASS = public VarNode

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#include "JVMVarNode.h"
#include "VarCmd.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

jvm::VarNode::VarNode			// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	::VarNode(shouldUpdate)
{
}

jvm::VarNode::VarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
	:
	::VarNode(parent, name, value)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

jvm::VarNode::~VarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
jvm::VarNode::GetFullName
	(
	bool* isPointer
	)
	const
{
	return GetFullNameForCFamilyLanguage(isPointer);
}
