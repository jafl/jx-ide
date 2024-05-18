/******************************************************************************
 ThreadNode.cpp

	BASE CLASS = public ThreadNode

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#include "jvm/ThreadNode.h"
#include "jvm/GetThreadNameCmd.h"
#include "jvm/GetThreadParentCmd.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

jvm::ThreadNode::ThreadNode
	(
	const Type		type,
	const JUInt64	id
	)
	:
	::ThreadNode(id, JString::empty, JString::empty, 0),
	itsType(type)
{
	if (id != kRootThreadGroupID)
	{
		dynamic_cast<Link&>(*GetLink()).ThreadCreated(this);

		Command* cmd = jnew GetThreadNameCmd(this);
	}
}

// constructor for search target

jvm::ThreadNode::ThreadNode
	(
	const JUInt64 id
	)
	:
	::ThreadNode(id, JString::empty, JString::empty, 0),
	itsType(kThreadType)
{
}

// constructor for nodes owned by ThreadsDir

jvm::ThreadNode::ThreadNode
	(
	const ThreadNode& node
	)
	:
	::ThreadNode(node.GetID(), node.GetName(), JString::empty, 0),
	itsType(node.GetType())
{
	if (itsType == kGroupType)
	{
		ShouldBeOpenable(true);
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

jvm::ThreadNode::~ThreadNode()
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link != nullptr)	// when switching debugger type, it won't be a Link
	{
		link->ThreadDeleted(this);
	}
}

/******************************************************************************
 NameChanged (virtual protected)

	Find our parent, if we don't already have one.

 ******************************************************************************/

void
jvm::ThreadNode::NameChanged()
{
	::ThreadNode::NameChanged();

	if (!HasParent())
	{
		Command* cmd = jnew GetThreadParentCmd(this);
	}
}

/******************************************************************************
 FindParent

 *****************************************************************************/

void
jvm::ThreadNode::FindParent
	(
	const JUInt64 id
	)
{
	auto& link = dynamic_cast<Link&>(*GetLink());

	ThreadNode* parent;
	if (!link.FindThread(id, &parent))
	{
		parent = jnew ThreadNode(kGroupType, id);
	}

	parent->AppendThread(this);
}

/******************************************************************************
 CompareID (static)

 ******************************************************************************/

std::weak_ordering
jvm::ThreadNode::CompareID
	(
	ThreadNode* const & t1,
	ThreadNode* const & t2
	)
{
	return t1->GetID() <=> t2->GetID();
}
