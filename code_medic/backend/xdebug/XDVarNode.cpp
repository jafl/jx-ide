/******************************************************************************
 XDVarNode.cpp

	BASE CLASS = public VarNode

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "XDVarNode.h"
#include "VarCommand.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

XDVarNode::XDVarNode				// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	VarNode(shouldUpdate)
{
}

XDVarNode::XDVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
	:
	VarNode(parent, name, value)
{
	itsFullName = fullName;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

XDVarNode::~XDVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

 ******************************************************************************/

JString
XDVarNode::GetFullName
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
XDVarNode::NameChanged()
{
	if (HasTree() && GetDepth() == 1)
	{
		itsFullName = GetName();
	}

	VarNode::NameChanged();
}
