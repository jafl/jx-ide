/******************************************************************************
 JVMGetIDSizes.cpp

	BASE CLASS = Command

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetIDSizes.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetIDSizes::JVMGetIDSizes()
	:
	Command("", true, false)
{
	Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetIDSizes::~JVMGetIDSizes()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetIDSizes::Starting()
{
	Command::Starting();

	dynamic_cast<JVMLink*>(GetLink())->Send(this,
		JVMLink::kVirtualMachineCmdSet, JVMLink::kVMIDSizesCmd, nullptr, 0);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetIDSizes::HandleSuccess
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
	assert( msg->GetDataLength() == 20 );

	const JSize fieldIDSize   = JVMSocket::Unpack4(data);
	const JSize methodIDSize  = JVMSocket::Unpack4(data+4);
	const JSize objectIDSize  = JVMSocket::Unpack4(data+8);
	const JSize refTypeIDSize = JVMSocket::Unpack4(data+12);
	const JSize frameIDSize   = JVMSocket::Unpack4(data+16);

	link->SetIDSizes(fieldIDSize, methodIDSize, objectIDSize, refTypeIDSize, frameIDSize);
}
