/******************************************************************************
 ThreadNode.cpp

	BASE CLASS = public JNamedTreeNode

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#include "ThreadNode.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

ThreadNode::ThreadNode
	(
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
}

/******************************************************************************
 Destructor

 *****************************************************************************/

ThreadNode::~ThreadNode()
{
}
