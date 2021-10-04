/******************************************************************************
 JVMGetClassInfoCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "JVMGetClassInfoCmd.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetClassInfoCmd::JVMGetClassInfoCmd
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

JVMGetClassInfoCmd::~JVMGetClassInfoCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetClassInfoCmd::Starting()
{
	Command::Starting();

	auto* link = dynamic_cast<JVMLink*>(GetLink());

	const JSize length  = link->GetObjectIDSize();
	auto* data = (unsigned char*) calloc(length, 1);
	assert( data != nullptr );

	JVMSocket::Pack(length, itsID, data);

	link->Send(this,
		JVMLink::kReferenceTypeCmdSet, JVMLink::kRTSignatureCmd, data, length);

	free(data);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetClassInfoCmd::HandleSuccess
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

	const unsigned char* data = msg->GetData();

	JSize count;
	const JString sig = JVMSocket::UnpackString(data, &count);

	link->AddClass(itsID, sig);
}
