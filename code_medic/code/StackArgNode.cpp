/******************************************************************************
 StackArgNode.cpp

	BASE CLASS = public JNamedTreeNode

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "StackArgNode.h"
#include "StackFrameNode.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

StackArgNode::StackArgNode
	(
	StackFrameNode*	parent,
	const JString&	name,
	const JString&	value
	)
	:
	JNamedTreeNode(nullptr, name, false),
	itsValue(value)
{
	assert( parent != nullptr );
	parent->AppendArg(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

StackArgNode::~StackArgNode()
{
}
