/******************************************************************************
 JVMGetThreadParentCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "JVMGetThreadParentCmd.h"
#include "JVMThreadNode.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThreadParentCmd::JVMGetThreadParentCmd
	(
	JVMThreadNode*	node,
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

JVMGetThreadParentCmd::~JVMGetThreadParentCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetThreadParentCmd::Starting()
{
	Command::Starting();

	if (itsNode != nullptr)
	{
		auto* link = dynamic_cast<JVMLink*>(GetLink());

		const JSize length  = link->GetObjectIDSize();
		auto* data = (unsigned char*) calloc(length, 1);
		assert( data != nullptr );

		JVMSocket::Pack(length, itsNode->GetID(), data);

		const bool isGroup = itsNode->GetType() == JVMThreadNode::kGroupType;
		link->Send(this,
			isGroup ? JVMLink::kThreadGroupReferenceCmdSet : JVMLink::kThreadReferenceCmdSet,
			isGroup ? JVMLink::kTGParentCmd : JVMLink::kTThreadGroupCmd,
			data, length);

		free(data);
	}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThreadParentCmd::HandleSuccess
	(
	const JString& origData
	)
{
	auto* link = dynamic_cast<JVMLink*>(GetLink());
	const JVMSocket::MessageReady* msg;
	if (!link->GetLatestMessageFromJVM(&msg))
	{
		return;
	}

	if (itsNode != nullptr && !itsCheckOnlyFlag)
	{
		const unsigned char* data = msg->GetData();

		const JUInt64 id = JVMSocket::Unpack(link->GetObjectIDSize(), data);

		itsNode->FindParent(id);
	}
}

/******************************************************************************
 HandleFailure (virtual protected)

	The thread or group no longer exists.

 *****************************************************************************/

void
JVMGetThreadParentCmd::HandleFailure()
{
	jdelete itsNode;
}
