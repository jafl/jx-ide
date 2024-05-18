/******************************************************************************
 GetIDSizesCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/GetIDSizesCmd.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetIDSizesCmd::GetIDSizesCmd()
	:
	Command("", true, false)
{
	Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetIDSizesCmd::~GetIDSizesCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
jvm::GetIDSizesCmd::Starting()
{
	Command::Starting();

	dynamic_cast<Link&>(*GetLink()).Send(this,
		Link::kVirtualMachineCmdSet, Link::kVMIDSizesCmd, nullptr, 0);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetIDSizesCmd::HandleSuccess
	(
	const JString& origData
	)
{
	auto& link = dynamic_cast<Link&>(*GetLink());
	const Socket::MessageReady* msg;
	if (!link.GetLatestMessageFromJVM(&msg))
	{
		return;
	}

	const unsigned char* data = msg->GetData();
	assert( msg->GetDataLength() == 20 );

	const JSize fieldIDSize   = Socket::Unpack4(data);
	const JSize methodIDSize  = Socket::Unpack4(data+4);
	const JSize objectIDSize  = Socket::Unpack4(data+8);
	const JSize refTypeIDSize = Socket::Unpack4(data+12);
	const JSize frameIDSize   = Socket::Unpack4(data+16);

	link.SetIDSizes(fieldIDSize, methodIDSize, objectIDSize, refTypeIDSize, frameIDSize);
}
