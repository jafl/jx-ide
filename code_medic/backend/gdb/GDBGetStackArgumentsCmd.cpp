/******************************************************************************
 GDBGetStackArgumentsCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "GDBGetStackArgumentsCmd.h"
#include "StackFrameNode.h"
#include "StackArgNode.h"
#include "GDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetStackArgumentsCmd::GetStackArgumentsCmd
	(
	JTree* tree
	)
	:
	Command("-stack-list-arguments 1", false, true),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetStackArgumentsCmd::~GetStackArgumentsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex framePattern =
	"frame=\\{"
	"level=\"(?P<FrameIndex>[[:digit:]]+)\""
	",args=\\[";

static const JRegex refPattern = "@0x[[:xdigit:]]+:";

void
gdb::GetStackArgumentsCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	JTreeNode* root        = itsTree->GetRoot();
	const JSize frameCount = root->GetChildCount();
	if (frameCount == 0)
	{
		return;
	}

	const JString& data = GetLastResult();
	std::istringstream stream(data.GetBytes());

	JPtrArray< JStringPtrMap<JString> > argList(JPtrArrayT::kDeleteAll);

	JStringIterator iter(data);
	JUtf8Character c;
	while (iter.Next(framePattern))
	{
		if (iter.Next(&c, JStringIterator::kStay) && c == ']')
		{
			continue;
		}

		JIndex frameIndex;
		bool ok = iter.GetLastMatch().GetSubstring("FrameIndex").ConvertToUInt(&frameIndex);
		assert( ok );

		auto* frameNode =
			dynamic_cast<StackFrameNode*>(root->GetChild(frameCount - frameIndex));
		assert( frameNode != nullptr );

		stream.seekg(iter.GetLastMatch().GetUtf8ByteRange().last);
		if (!Link::ParseMapArray(stream, &argList))
		{
			Link::Log("invalid stack argument list");
			break;
		}

		const JSize count = argList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			JStringPtrMap<JString>* arg = argList.GetElement(i);
			JString *name, *value;
			if (!arg->GetElement("name", &name) ||
				!arg->GetElement("value", &value))
			{
				Link::Log("invalid stack argument");
				continue;
			}

			JStringIterator iter(value);
			if (iter.Next(refPattern))
			{
				iter.SkipPrev();
				iter.RemoveAllNext();
			}
			iter.Invalidate();

			auto* argNode = jnew StackArgNode(frameNode, *name, *value);
			assert( argNode != nullptr );
		}
	}
}
