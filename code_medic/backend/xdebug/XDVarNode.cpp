/******************************************************************************
 XDVarNode.cpp

	BASE CLASS = public VarNode

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "XDVarNode.h"
#include "VarCmd.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

xdebug::VarNode::VarNode				// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	::VarNode(shouldUpdate)
{
}

xdebug::VarNode::VarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
	:
	::VarNode(parent, name, value)
{
	itsFullName = fullName;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

xdebug::VarNode::~VarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

 ******************************************************************************/

JString
xdebug::VarNode::GetFullName
	(
	bool* isPointer
	)
	const
{
	return itsFullName;
}

/******************************************************************************
 NameChanged (virtual protected)

 ******************************************************************************/

void
xdebug::VarNode::NameChanged()
{
	if (HasTree() && GetDepth() == 1)
	{
		itsFullName = GetName();
	}

	VarNode::NameChanged();
}
