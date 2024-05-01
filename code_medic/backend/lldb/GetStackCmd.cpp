/******************************************************************************
 GetStackCmd.cpp

	BASE CLASS = GetStackCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/GetStackCmd.h"
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
#include "lldb/Link.h"	// must be after X11 includes: Status
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kFrameIndexWidth = 2;	// width of frame index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetStackCmd::GetStackCmd
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

lldb::GetStackCmd::~GetStackCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex assertPattern("^(JAssert|__assert(_[^[:space:]]+)?)\\(");

void
lldb::GetStackCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	SBThread t = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread();
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
		SBFrame f = t.GetFrameAtIndex(i);

		frameName = JString(i);
		while (frameName.GetCharacterCount() < kFrameIndexWidth)
		{
			frameName.Prepend("0");
		}
		frameName += ":  ";

		const JUtf8Byte* name = f.GetFunctionName();
		frameName += (name == nullptr ? "?" : name);

		JIndex lineIndex      = 0;
		SBLineEntry e   = f.GetLineEntry();
		SBFileSpec file = e.GetFileSpec();
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

		SBValueList args = f.GetVariables(true, false, false, true, eDynamicDontRunTarget);
		if (!args.IsValid())
		{
			continue;
		}

		for (JUnsignedOffset i=0; i<args.GetSize(); i++)
		{
			SBValue v = args.GetValueAtIndex(i);

			if (v.GetName() != nullptr)
			{
				jnew StackArgNode(node,
					JString(v.GetName(), JString::kNoCopy),
					JString(v.GetValue() == nullptr ? "null" : v.GetValue(), JString::kNoCopy));
			}
		}
	}

	GetWidget()->FinishedLoading(initFrameIndex);
}
