/******************************************************************************
 GetStackCmd.cpp

	BASE CLASS = GetStackCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/GetStackCmd.h"
#include "StackFrameNode.h"
#include "StackWidget.h"
#include "xdebug/Link.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kFrameIndexWidth = 2;	// width of frame index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::GetStackCmd::GetStackCmd
	(
	JTree*			tree,
	StackWidget*	widget
	)
	:
	::GetStackCmd("stack_get", tree, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::GetStackCmd::~GetStackCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::GetStackCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
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
		stackRoot->Prepend(node);

		frame = frame->next;
	}

	GetWidget()->FinishedLoading(0);
	GetWidget()->SwitchToFrame(0);
}
