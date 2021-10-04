/******************************************************************************
 JVMGetThreadNameCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "JVMGetThreadNameCmd.h"
#include "JVMThreadNode.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThreadNameCmd::JVMGetThreadNameCmd
	(
	JVMThreadNode* node
	)
	:
	Command("", true, false),
	itsNode(node)
{
	ClearWhenGoingAway(itsNode, &itsNode);
	Command::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThreadNameCmd::~JVMGetThreadNameCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetThreadNameCmd::Starting()
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
			isGroup ? JVMLink::kTGNameCmd : JVMLink::kTNameCmd,
			data, length);

		free(data);
	}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThreadNameCmd::HandleSuccess
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

	if (itsNode != nullptr)
	{
		const unsigned char* data = msg->GetData();

		JSize count;
		const JString name = JVMSocket::UnpackString(data, &count);

		itsNode->SetName(name);
	}
}

/******************************************************************************
 HandleFailure (virtual protected)

	The thread or group no longer exists.

 *****************************************************************************/

void
JVMGetThreadNameCmd::HandleFailure()
{
	jdelete itsNode;
}
