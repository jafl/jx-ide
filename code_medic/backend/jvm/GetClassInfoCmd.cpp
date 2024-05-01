/******************************************************************************
 GetClassInfoCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "jvm/GetClassInfoCmd.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetClassInfoCmd::GetClassInfoCmd
	(
	const JUInt64 id
	)
	:
	Command("", true, false),
	itsID(id)
{
	Command::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetClassInfoCmd::~GetClassInfoCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
jvm::GetClassInfoCmd::Starting()
{
	Command::Starting();

	auto* link = dynamic_cast<Link*>(GetLink());

	const JSize length = link->GetObjectIDSize();

	auto* data = (unsigned char*) calloc(length, 1);
	assert( data != nullptr );

	Socket::Pack(length, itsID, data);

	link->Send(this,
		Link::kReferenceTypeCmdSet, Link::kRTSignatureCmd, data, length);

	free(data);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetClassInfoCmd::HandleSuccess
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

	const unsigned char* data = msg->GetData();

	JSize count;
	const JString sig = Socket::UnpackString(data, &count);

	link->AddClass(itsID, sig);
}
