/******************************************************************************
 GetThreadParentCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "jvm/GetThreadParentCmd.h"
#include "jvm/ThreadNode.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetThreadParentCmd::GetThreadParentCmd
	(
	ThreadNode*	node,
	const bool	checkOnly
	)
	:
	Command("", true, false),
	itsNode(node),
	itsCheckOnlyFlag(checkOnly)
{
	ClearWhenGoingAway(itsNode, &itsNode);
	Command::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetThreadParentCmd::~GetThreadParentCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
jvm::GetThreadParentCmd::Starting()
{
	Command::Starting();

	if (itsNode != nullptr)
	{
		auto* link = dynamic_cast<Link*>(GetLink());

		const JSize length  = link->GetObjectIDSize();
		auto* data = (unsigned char*) calloc(length, 1);
		assert( data != nullptr );

		Socket::Pack(length, itsNode->GetID(), data);

		const bool isGroup = itsNode->GetType() == ThreadNode::kGroupType;
		link->Send(this,
			isGroup ? Link::kThreadGroupReferenceCmdSet : Link::kThreadReferenceCmdSet,
			isGroup ? Link::kTGParentCmd : Link::kTThreadGroupCmd,
			data, length);

		free(data);
	}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetThreadParentCmd::HandleSuccess
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

	if (itsNode != nullptr && !itsCheckOnlyFlag)
	{
		const unsigned char* data = msg->GetData();

		const JUInt64 id = Socket::Unpack(link->GetObjectIDSize(), data);

		itsNode->FindParent(id);
	}
}

/******************************************************************************
 HandleFailure (virtual protected)

	The thread or group no longer exists.

 *****************************************************************************/

void
jvm::GetThreadParentCmd::HandleFailure()
{
	jdelete itsNode;
}
