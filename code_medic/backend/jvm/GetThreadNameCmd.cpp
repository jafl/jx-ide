/******************************************************************************
 GetThreadNameCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "jvm/GetThreadNameCmd.h"
#include "jvm/ThreadNode.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetThreadNameCmd::GetThreadNameCmd
	(
	ThreadNode* node
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

jvm::GetThreadNameCmd::~GetThreadNameCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
jvm::GetThreadNameCmd::Starting()
{
	Command::Starting();

	if (itsNode != nullptr)
	{
		auto* link = dynamic_cast<Link*>(GetLink());

		const JSize length = link->GetObjectIDSize();

		auto* data = (unsigned char*) calloc(length, 1);
		assert( data != nullptr );

		Socket::Pack(length, itsNode->GetID(), data);

		const bool isGroup = itsNode->GetType() == ThreadNode::kGroupType;
		link->Send(this,
			isGroup ? Link::kThreadGroupReferenceCmdSet : Link::kThreadReferenceCmdSet,
			isGroup ? Link::kTGNameCmd : Link::kTNameCmd,
			data, length);

		free(data);
	}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetThreadNameCmd::HandleSuccess
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

	if (itsNode != nullptr)
	{
		const unsigned char* data = msg->GetData();

		JSize count;
		const JString name = Socket::UnpackString(data, &count);

		itsNode->SetName(name);
	}
}

/******************************************************************************
 HandleFailure (virtual protected)

	The thread or group no longer exists.

 *****************************************************************************/

void
jvm::GetThreadNameCmd::HandleFailure()
{
	jdelete itsNode;
}
