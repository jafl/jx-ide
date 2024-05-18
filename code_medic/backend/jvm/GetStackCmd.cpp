/******************************************************************************
 GetStackCmd.cpp

	BASE CLASS = GetStackCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/GetStackCmd.h"
#include "jvm/StackFrameNode.h"
#include "StackWidget.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetStackCmd::GetStackCmd
	(
	JTree*			tree,
	StackWidget*	widget
	)
	:
	::GetStackCmd(JString::empty, tree, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetStackCmd::~GetStackCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
jvm::GetStackCmd::Starting()
{
	::GetStackCmd::Starting();

	auto& link         = dynamic_cast<Link&>(*GetLink());
	const JSize length = link.GetObjectIDSize();
	const JSize size   = length+8;

	auto* data = (unsigned char*) calloc(size, 1);
	assert( data != nullptr );

	unsigned char* d = data;
	Socket::Pack(length, link.GetCurrentThreadID(), d);
	d += length;

	Socket::Pack4(0, d);
	d += 4;

	Socket::Pack4(-1, d);

	link.Send(this, Link::kThreadReferenceCmdSet, Link::kTFramesCmd, data, size);
	free(data);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetStackCmd::HandleSuccess
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

	link.FlushFrameList();

	JTreeNode* root = GetTree()->GetRoot();

	const unsigned char* data  = msg->GetData();
	const JSize frameIDLength  = link.GetFrameIDSize();
	const JSize classIDLength  = link.GetObjectIDSize();
	const JSize methodIDLength = link.GetMethodIDSize();

	const JSize frameCount = Socket::Unpack4(data);
	data                  += 4;

	for (JIndex i=1; i<=frameCount; i++)
	{
		const JSize id = Socket::Unpack(frameIDLength, data);
		data          += frameIDLength;

		const JIndex type = *data;
		data++;

		const JUInt64 classID = Socket::Unpack(classIDLength, data);
		data                 += classIDLength;

		const JUInt64 methodID = Socket::Unpack(methodIDLength, data);
		data                  += methodIDLength;

		const JUInt64 offset = Socket::Unpack8(data);
		data                += 8;

		auto* node =
			jnew StackFrameNode(root, id, classID, methodID, offset);
		root->Prepend(node);

		link.AddFrame(id, classID, methodID, offset);
	}

	GetWidget()->FinishedLoading(0);
}
