/******************************************************************************
 GetStackCmd.cpp

	BASE CLASS = GetStackCmd

	Copyright (C) 2001-09 by John Lindal.

 ******************************************************************************/

#include "gdb/GetStackCmd.h"
#include "gdb/GetStackArgumentsCmd.h"
#include "StackFrameNode.h"
#include "StackWidget.h"
#include "gdb/Link.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

const JSize kFrameIndexWidth = 2;	// width of frame index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetStackCmd::GetStackCmd
	(
	JTree*			tree,
	StackWidget*	widget
	)
	:
	::GetStackCmd("-stack-list-frames", tree, widget)
{
	itsArgsCmd = jnew GetStackArgumentsCmd(tree);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetStackCmd::~GetStackCmd()
{
	jdelete itsArgsCmd;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex framePattern("\\bframe=\\{");
static const JRegex assertPattern("^(JAssert|__assert(_[^[:space:]]+)?)$");

void
gdb::GetStackCmd::HandleSuccess
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
		if (!Link::ParseMap(stream, &map))
		{
			Link::Log("invalid data map");
			break;
		}
		iter.MoveTo(JStringIterator::kStartAfterChar, (std::streamoff) stream.tellg());

		JString* s;
		JIndex frameIndex;
		if (!map.GetItem("level", &s))
		{
			Link::Log("missing frame index");
			continue;
		}
		if (!s->ConvertToUInt(&frameIndex))
		{
			Link::Log("frame index is not integer");
			continue;
		}

		frameName = *s;
		while (frameName.GetCharacterCount() < kFrameIndexWidth)
		{
			frameName.Prepend("0");
		}
		frameName += ":  ";

		JString* fnName;
		if (!map.GetItem("func", &fnName))
		{
			Link::Log("missing function name");
			continue;
		}
		frameName += *fnName;

		if (map.GetItem("file", &s))
		{
			fileName = *s;
		}

		JIndex lineIndex = 0;
		if (map.GetItem("line", &s) &&
			!s->ConvertToUInt(&lineIndex))
		{
			Link::Log("line number is not integer");
			continue;
		}

		auto* node =
			jnew StackFrameNode(root, frameIndex, frameName,
								fileName, lineIndex);
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
