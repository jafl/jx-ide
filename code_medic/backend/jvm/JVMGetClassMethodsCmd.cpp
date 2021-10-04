/******************************************************************************
 JVMGetClassMethodsCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "JVMGetClassMethodsCmd.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetClassMethodsCmd::GetClassMethodsCmd
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

jvm::GetClassMethodsCmd::~GetClassMethodsCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
jvm::GetClassMethodsCmd::Starting()
{
	Command::Starting();

	auto* link = dynamic_cast<Link*>(GetLink());

	const JSize length  = link->GetObjectIDSize();
	auto* data = (unsigned char*) calloc(length, 1);
	assert( data != nullptr );

	Socket::Pack(length, itsID, data);

	link->Send(this,
		Link::kReferenceTypeCmdSet, Link::kRTMethodsCmd, data, length);

	free(data);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetClassMethodsCmd::HandleSuccess
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
	const JSize idLength      = link->GetMethodIDSize();

	const JSize count = Socket::Unpack4(data);
	data             += 4;

	JString name, sig;
	for (JIndex i=1; i<=count; i++)
	{
		const JUInt64 methodID = Socket::Unpack(idLength, data);
		data                  += idLength;

		JSize length;
		name  = Socket::UnpackString(data, &length);
		data += length;

		sig   = Socket::UnpackString(data, &length);
		data += length;

		const JSize bits = Socket::Unpack4(data);
		data            += 4;

		link->AddMethod(itsID, methodID, name);
	}
}
