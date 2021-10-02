/******************************************************************************
 LLDBGetStack.cpp

	BASE CLASS = GetStack

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetStack.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBLineEntry.h"
#include "lldb/API/SBFileSpec.h"
#include "lldb/API/SBValueList.h"
#include "lldb/API/SBValue.h"
#include "StackFrameNode.h"
#include "StackArgNode.h"
#include "StackWidget.h"
#include "globals.h"
#include "LLDBLink.h"	// must be after X11 includes: Status
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kFrameIndexWidth = 2;	// width of frame index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetStack::LLDBGetStack
	(
	JTree*			tree,
	StackWidget*	widget
	)
	:
	GetStack(JString::empty, tree, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetStack::~LLDBGetStack()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex assertPattern = "^(JAssert|__assert(_[^[:space:]]+)?)\\(";

void
LLDBGetStack::HandleSuccess
	(
	const JString& cmdData
	)
{
	auto* link = dynamic_cast<LLDBLink*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	lldb::SBThread t = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (!t.IsValid())
	{
		return;
	}

	JTreeNode* root       = GetTree()->GetRoot();
	JIndex initFrameIndex = 0;

	const JSize frameCount = t.GetNumFrames();
	JString frameName, fileName;
	bool selectNextFrame = false;
	for (JUnsignedOffset i=0; i<frameCount; i++)
	{
		lldb::SBFrame f = t.GetFrameAtIndex(i);

		frameName = JString((JUInt64) i);
		while (frameName.GetCharacterCount() < kFrameIndexWidth)
		{
			frameName.Prepend("0");
		}
		frameName += ":  ";

		const JUtf8Byte* name = f.GetFunctionName();
		frameName += (name == nullptr ? "?" : name);

		JIndex lineIndex      = 0;
		lldb::SBLineEntry e   = f.GetLineEntry();
		lldb::SBFileSpec file = e.GetFileSpec();
		if (file.IsValid())
		{
			fileName = JCombinePathAndName(
				JString(file.GetDirectory(), JString::kNoCopy),
				JString(file.GetFilename(), JString::kNoCopy));

			lineIndex = e.GetLine();
		}
		else
		{
			fileName.Clear();
		}

		auto* node =
			jnew StackFrameNode(root, f.GetFrameID(), frameName,
								 fileName, lineIndex);
		assert( node != nullptr );
		root->Prepend(node);

		if (selectNextFrame)
		{
			initFrameIndex  = f.GetFrameID();
			selectNextFrame = false;
		}
		else if (f.GetFunctionName() != nullptr && assertPattern.Match(JString(f.GetFunctionName(), JString::kNoCopy)))
		{
			selectNextFrame = true;
		}

		// arguments

		lldb::SBValueList args = f.GetVariables(true, false, false, true, lldb::eDynamicDontRunTarget);
		if (!args.IsValid())
		{
			continue;
		}

		for (JUnsignedOffset i=0; i<args.GetSize(); i++)
		{
			lldb::SBValue v = args.GetValueAtIndex(i);

			if (v.GetName() != nullptr)
			{
				auto* argNode = jnew StackArgNode(node,
					JString(v.GetName(), JString::kNoCopy),
					JString(v.GetValue() == nullptr ? "null" : v.GetValue(), JString::kNoCopy));
				assert( argNode != nullptr );
			}
		}
	}

	GetWidget()->FinishedLoading(initFrameIndex);
}
