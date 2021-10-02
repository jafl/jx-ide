/******************************************************************************
 GDBVarNode.cpp

	BASE CLASS = public VarNode

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "GDBVarNode.h"
#include "VarCommand.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBVarNode::GDBVarNode				// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	VarNode(shouldUpdate)
{
}

GDBVarNode::GDBVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	value
	)
	:
	VarNode(parent, name, value)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBVarNode::~GDBVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
GDBVarNode::GetFullName
	(
	bool* isPointer
	)
	const
{
	return GetFullNameForCFamilyLanguage(isPointer);
}
