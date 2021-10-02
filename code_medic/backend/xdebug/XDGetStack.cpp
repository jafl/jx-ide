/******************************************************************************
 XDGetStack.cpp

	BASE CLASS = GetStack

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetStack.h"
#include "StackFrameNode.h"
#include "StackWidget.h"
#include "XDLink.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kFrameIndexWidth = 2;	// width of frame index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetStack::XDGetStack
	(
	JTree*			tree,
	StackWidget*	widget
	)
	:
	GetStack(JString("stack_get", JString::kNoCopy), tree, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetStack::~XDGetStack()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetStack::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<XDLink*>(GetLink());
	xmlNode* root;
	if (link == nullptr || !link->GetParsedData(&root))
	{
		return;
	}

	JTreeNode* stackRoot = GetTree()->GetRoot();

	xmlNode* frame  = root->children;
	JString frameIndexStr, fileName, lineStr, frameName, path, name;
	while (frame != nullptr)
	{
		frameIndexStr = JGetXMLNodeAttr(frame, "level");
		JUInt frameIndex;
		bool ok = frameIndexStr.ConvertToUInt(&frameIndex);
		assert( ok );

		fileName = JGetXMLNodeAttr(frame, "filename");

		lineStr = JGetXMLNodeAttr(frame, "lineno");
		JUInt lineIndex;
		ok = lineStr.ConvertToUInt(&lineIndex);
		assert( ok );

		name = JGetXMLNodeAttr(frame, "where");
		if (name.IsEmpty())
		{
			JSplitPathAndName(fileName, &path, &name);
		}

		frameName = frameIndexStr;
		while (frameName.GetCharacterCount() < kFrameIndexWidth)
		{
			frameName.Prepend("0");
		}
		frameName += ":  ";
		frameName += name;

		auto* node =
			jnew StackFrameNode(stackRoot, frameIndex, frameName,
								 fileName, lineIndex);
		assert( node != nullptr );
		stackRoot->Prepend(node);

		frame = frame->next;
	}

	GetWidget()->FinishedLoading(0);
	GetWidget()->SwitchToFrame(0);
}
