/******************************************************************************
 StackFrameNode.cpp

	BASE CLASS = public JNamedTreeNode

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "StackFrameNode.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

StackFrameNode::StackFrameNode
	(
	JTreeNode*		parent,
	const JUInt64	id,
	const JString&	name,
	const JString&	fileName,
	const JIndex	lineIndex
	)
	:
	JNamedTreeNode(nullptr, name, false),
	itsID(id),
	itsFileName(fileName),
	itsLineIndex(lineIndex)
{
	assert( parent != nullptr );
	parent->Append(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

StackFrameNode::~StackFrameNode()
{
}
