/******************************************************************************
 VarNode.cpp

	BASE CLASS = public VarNode

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "gdb/VarNode.h"
#include "VarCmd.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

gdb::VarNode::VarNode				// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	::VarNode(shouldUpdate)
{
}

gdb::VarNode::VarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	value
	)
	:
	::VarNode(parent, name, value)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

gdb::VarNode::~VarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
gdb::VarNode::GetFullName
	(
	bool* isPointer
	)
	const
{
	return GetFullNameForCFamilyLanguage(isPointer);
}

/******************************************************************************
 GetFullNameWithCast (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
gdb::VarNode::GetFullNameWithCast
	(
	bool* isPointer
	)
	const
{
	JString s = GetFullNameForCFamilyLanguage(isPointer), t;
	if (GetType(&t))
	{
		s.Prepend(") ");
		s.Prepend(t);
		s.Prepend("(");
	}
	return s;
}
