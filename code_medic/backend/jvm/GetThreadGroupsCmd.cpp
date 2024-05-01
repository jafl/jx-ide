/******************************************************************************
 GetThreadGroupsCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "jvm/GetThreadGroupsCmd.h"
#include "jvm/ThreadNode.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetThreadGroupsCmd::GetThreadGroupsCmd
	(
	JTreeNode*	root,
	ThreadNode*	parent
	)
	:
	Command("", true, false),
	itsRoot(root),
	itsParent(parent)
{
	ListenTo(itsRoot);
	if (itsParent != nullptr)
	{
		ListenTo(itsParent);
	}

	Command::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetThreadGroupsCmd::~GetThreadGroupsCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
jvm::GetThreadGroupsCmd::Starting()
{
	Command::Starting();

	if (itsRoot == nullptr)
	{
		return;
	}

	auto* link = dynamic_cast<Link*>(GetLink());
	if (itsParent == nullptr)
	{
		link->Send(this,
			Link::kVirtualMachineCmdSet, Link::kVMTopLevelThreadGroupsCmd, nullptr, 0);
	}
	else
	{
		const JSize length = link->GetObjectIDSize();

		auto* data = (unsigned char*) calloc(length, 1);
		assert( data != nullptr );

		Socket::Pack(length, itsParent->GetID(), data);

		link->Send(this,
			Link::kThreadGroupReferenceCmdSet, Link::kTGChildrenCmd, data, length);

		free(data);
	}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetThreadGroupsCmd::HandleSuccess
	(
	const JString& origData
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	const Socket::MessageReady* msg;
	if (!link->GetLatestMessageFromJVM(&msg))
	{
		return;
	}

	if (itsRoot == nullptr)
	{
		return;
	}

	const unsigned char* data = msg->GetData();
	const JSize idLength      = link->GetObjectIDSize();

	if (itsParent != nullptr)
	{
		const JSize threadCount = Socket::Unpack4(data);
		data                   += 4;

		for (JIndex i=1; i<=threadCount; i++)
		{
			const JUInt64 id = Socket::Unpack(idLength, data);
			data            += idLength;

			ThreadNode* node;
			if (!link->FindThread(id, &node))	// might be created by ThreadStartEvent
			{
				node = jnew ThreadNode(ThreadNode::kThreadType, id);

				itsParent->AppendThread(node);
			}
		}
	}

	const JSize groupCount = Socket::Unpack4(data);
	data                  += 4;

	for (JIndex i=1; i<=groupCount; i++)
	{
		const JUInt64 id = Socket::Unpack(idLength, data);
		data            += idLength;

		ThreadNode* node;
		if (!link->FindThread(id, &node))	// might be created by ThreadStartEvent
		{
			node = jnew ThreadNode(ThreadNode::kGroupType, id);

			if (itsParent != nullptr)
			{
				itsParent->AppendThread(node);
			}
			else
			{
				itsRoot->Append(node);
			}
		}
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

	Warning:	Since this function may be called from within a -chain-
				of destructors, it is not usually safe to do anything
				inside this function other than directly changing
				instance variables (e.g. setting pointers to nullptr).

	This function is not pure virtual because not all classes will want
	to implement it.

 ******************************************************************************/

void
jvm::GetThreadGroupsCmd::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsRoot || sender == itsParent)
	{
		itsRoot   = nullptr;
		itsParent = nullptr;
	}

	JBroadcaster::ReceiveGoingAway(sender);
}
