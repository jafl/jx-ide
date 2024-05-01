/******************************************************************************
 StackFrameNode.cpp

	BASE CLASS = public StackFrameNode

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "jvm/StackFrameNode.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

jvm::StackFrameNode::StackFrameNode
	(
	JTreeNode*		parent,
	const JUInt64	id,
	const JUInt64	classID,
	const JUInt64	methodID,
	const JUInt64	codeOffset
	)
	:
	::StackFrameNode(parent, id, JString::empty, JString::empty, 0),
	itsClassID(classID),
	itsMethodID(methodID),
	itsCodeOffset(codeOffset)
{
	if (!UpdateNodeName())
	{
		ListenTo(GetLink());
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

jvm::StackFrameNode::~StackFrameNode()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
jvm::StackFrameNode::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(Link::kIDResolved))
	{
		auto* info = dynamic_cast<const Link::IDResolved*>(&message);
		assert( info != nullptr );

		if (info->GetID() == itsClassID || info->GetID() == itsMethodID)
		{
			UpdateNodeName();	// can't stop listening, because base class might be listening, too
		}
	}

	else
	{
		::StackFrameNode::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateNodeName

 *****************************************************************************/

bool
jvm::StackFrameNode::UpdateNodeName()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	JString c, m;
	if (link->GetClassName(itsClassID, &c) &&
		link->GetMethodName(itsClassID, itsMethodID, &m))
	{
		c += ".";
		c += m;
		SetName(c);
		return true;
	}

	if (GetName().IsEmpty())
	{
		c = JString(itsClassID) + ":" +
			JString(itsMethodID) + ":" +
			JString(itsCodeOffset);
		SetName(c);
	}
	return false;
}
