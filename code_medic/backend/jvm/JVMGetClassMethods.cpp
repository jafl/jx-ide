/******************************************************************************
 JVMGetClassMethods.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "JVMGetClassMethods.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetClassMethods::JVMGetClassMethods
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

JVMGetClassMethods::~JVMGetClassMethods()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetClassMethods::Starting()
{
	Command::Starting();

	auto* link = dynamic_cast<JVMLink*>(GetLink());

	const JSize length  = link->GetObjectIDSize();
	auto* data = (unsigned char*) calloc(length, 1);
	assert( data != nullptr );

	JVMSocket::Pack(length, itsID, data);

	link->Send(this,
		JVMLink::kReferenceTypeCmdSet, JVMLink::kRTMethodsCmd, data, length);

	free(data);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetClassMethods::HandleSuccess
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
	const JSize idLength      = link->GetMethodIDSize();

	const JSize count = JVMSocket::Unpack4(data);
	data             += 4;

	JString name, sig;
	for (JIndex i=1; i<=count; i++)
	{
		const JUInt64 methodID = JVMSocket::Unpack(idLength, data);
		data                  += idLength;

		JSize length;
		name  = JVMSocket::UnpackString(data, &length);
		data += length;

		sig   = JVMSocket::UnpackString(data, &length);
		data += length;

		const JSize bits = JVMSocket::Unpack4(data);
		data            += 4;

		link->AddMethod(itsID, methodID, name);
	}
}
