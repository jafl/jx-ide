/******************************************************************************
 GDBGetStack.cpp

	BASE CLASS = GetStack

	Copyright (C) 2001-09 by John Lindal.

 ******************************************************************************/

#include "GDBGetStack.h"
#include "GDBGetStackArguments.h"
#include "StackFrameNode.h"
#include "StackWidget.h"
#include "GDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

const JSize kFrameIndexWidth = 2;	// width of frame index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetStack::GDBGetStack
	(
	JTree*			tree,
	StackWidget*	widget
	)
	:
	GetStack(JString("-stack-list-frames", JString::kNoCopy), tree, widget)
{
	itsArgsCmd = jnew GDBGetStackArguments(tree);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetStack::~GDBGetStack()
{
	jdelete itsArgsCmd;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex framePattern  = "\\bframe=\\{";
static const JRegex assertPattern = "^(JAssert|__assert(_[^[:space:]]+)?)$";

void
GDBGetStack::HandleSuccess
	(
	const JString& cmdData
	)
{
	JTreeNode* root       = GetTree()->GetRoot();
	JIndex initFrameIndex = 0;

	const JString& data = GetLastResult();
	std::istringstream stream(data.GetBytes());

	JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
	JString frameName, fileName;
	bool selectNextFrame = false;

	JStringIterator iter(data);
	while (iter.Next(framePattern))
	{
		stream.seekg(iter.GetLastMatch().GetUtf8ByteRange().last);
		if (!GDBLink::ParseMap(stream, &map))
		{
			GetLink()->Log("invalid data map");
			break;
		}
		iter.MoveTo(kJIteratorStartAfter, (std::streamoff) stream.tellg());

		JString* s;
		JIndex frameIndex;
		if (!map.GetElement("level", &s))
		{
			GetLink()->Log("missing frame index");
			continue;
		}
		if (!s->ConvertToUInt(&frameIndex))
		{
			GetLink()->Log("frame index is not integer");
			continue;
		}

		frameName = *s;
		while (frameName.GetCharacterCount() < kFrameIndexWidth)
		{
			frameName.Prepend("0");
		}
		frameName += ":  ";

		JString* fnName;
		if (!map.GetElement("func", &fnName))
		{
			GetLink()->Log("missing function name");
			continue;
		}
		frameName += *fnName;

		if (map.GetElement("file", &s))
		{
			fileName = *s;
		}

		JIndex lineIndex = 0;
		if (map.GetElement("line", &s) &&
			!s->ConvertToUInt(&lineIndex))
		{
			GetLink()->Log("line number is not integer");
			continue;
		}

		auto* node =
			jnew StackFrameNode(root, frameIndex, frameName,
								  fileName, lineIndex);
		assert( node != nullptr );
		root->Prepend(node);

		if (selectNextFrame)
		{
			initFrameIndex  = frameIndex;
			selectNextFrame = false;
		}
		else if (assertPattern.Match(*fnName))
		{
			selectNextFrame = true;
		}
	}

	itsArgsCmd->Send();

	GetWidget()->FinishedLoading(initFrameIndex);
}
