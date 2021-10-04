/******************************************************************************
 JVMThreadNode.cpp

	BASE CLASS = public ThreadNode

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#include "JVMThreadNode.h"
#include "JVMGetThreadNameCmd.h"
#include "JVMGetThreadParentCmd.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JVMThreadNode::JVMThreadNode
	(
	const Type		type,
	const JUInt64	id
	)
	:
	ThreadNode(id, JString::empty, JString::empty, 0),
	itsType(type)
{
	if (id != kRootThreadGroupID)
	{
		dynamic_cast<JVMLink*>(GetLink())->ThreadCreated(this);

		Command* cmd = jnew JVMGetThreadNameCmd(this);
		assert( cmd != nullptr );
	}
}

// constructor for search target

JVMThreadNode::JVMThreadNode
	(
	const JUInt64 id
	)
	:
	ThreadNode(id, JString::empty, JString::empty, 0),
	itsType(kThreadType)
{
}

// constructor for nodes owned by ThreadsDir

JVMThreadNode::JVMThreadNode
	(
	const JVMThreadNode& node
	)
	:
	ThreadNode(node.GetID(), node.GetName(), JString::empty, 0),
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

JVMThreadNode::~JVMThreadNode()
{
	auto* link = dynamic_cast<JVMLink*>(GetLink());
	if (link != nullptr)	// when switching debugger type, it won't be a JVMLink
	{
		link->ThreadDeleted(this);
	}
}

/******************************************************************************
 NameChanged (virtual protected)

	Find our parent, if we don't already have one.

 ******************************************************************************/

void
JVMThreadNode::NameChanged()
{
	ThreadNode::NameChanged();

	if (!HasParent())
	{
		Command* cmd = jnew JVMGetThreadParentCmd(this);
		assert( cmd != nullptr );
	}
}

/******************************************************************************
 FindParent

 *****************************************************************************/

void
JVMThreadNode::FindParent
	(
	const JUInt64 id
	)
{
	auto* link = dynamic_cast<JVMLink*>(GetLink());

	JVMThreadNode* parent;
	if (!link->FindThread(id, &parent))
	{
		parent = jnew JVMThreadNode(kGroupType, id);
		assert( parent != nullptr );
	}

	parent->AppendThread(this);
}

/******************************************************************************
 CompareID (static)

 ******************************************************************************/

JListT::CompareResult
JVMThreadNode::CompareID
	(
	JVMThreadNode* const & t1,
	JVMThreadNode* const & t2
	)
{
	return JCompareUInt64(t1->GetID(), t2->GetID());
}
