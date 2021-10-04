/******************************************************************************
 JVMGetStackCmd.cpp

	BASE CLASS = GetStackCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetStackCmd.h"
#include "JVMStackFrameNode.h"
#include "StackWidget.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetStackCmd::JVMGetStackCmd
	(
	JTree*			tree,
	StackWidget*	widget
	)
	:
	GetStackCmd(JString::empty, tree, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetStackCmd::~JVMGetStackCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetStackCmd::Starting()
{
	Command::Starting();

	auto* link       = dynamic_cast<JVMLink*>(GetLink());
	const JSize length  = link->GetObjectIDSize();
	const JSize size    = length+8;
	auto* data = (unsigned char*) calloc(size, 1);
	assert( data != nullptr );

	unsigned char* d = data;
	JVMSocket::Pack(length, link->GetCurrentThreadID(), d);
	d += length;

	JVMSocket::Pack4(0, d);
	d += 4;

	JVMSocket::Pack4(-1, d);

	link->Send(this,
		JVMLink::kThreadReferenceCmdSet, JVMLink::kTFramesCmd, data, size);

	free(data);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetStackCmd::HandleSuccess
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

	link->FlushFrameList();

	JTreeNode* root = GetTree()->GetRoot();

	const unsigned char* data  = msg->GetData();
	const JSize frameIDLength  = link->GetFrameIDSize();
	const JSize classIDLength  = link->GetObjectIDSize();
	const JSize methodIDLength = link->GetMethodIDSize();

	const JSize frameCount = JVMSocket::Unpack4(data);
	data                  += 4;

	for (JIndex i=1; i<=frameCount; i++)
	{
		const JSize id = JVMSocket::Unpack(frameIDLength, data);
		data          += frameIDLength;

		const JIndex type = *data;
		data++;

		const JUInt64 classID = JVMSocket::Unpack(classIDLength, data);
		data                 += classIDLength;

		const JUInt64 methodID = JVMSocket::Unpack(methodIDLength, data);
		data                  += methodIDLength;

		const JUInt64 offset = JVMSocket::Unpack8(data);
		data                += 8;

		auto* node =
			jnew JVMStackFrameNode(root, id, classID, methodID, offset);
		assert( node != nullptr );
		root->Prepend(node);

		link->AddFrame(id, classID, methodID, offset);
	}

	GetWidget()->FinishedLoading(0);
}
