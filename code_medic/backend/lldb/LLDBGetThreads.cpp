/******************************************************************************
 LLDBGetThreads.cpp

	BASE CLASS = GetThreadsCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetThreads.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBStream.h"
#include "ThreadsWidget.h"
#include "ThreadNode.h"
#include "globals.h"
#include "LLDBLink.h"	// must be after X11 includes: Status
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kThreadIndexWidth = 2;	// width of thread index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetThreads::LLDBGetThreads
	(
	JTree*				tree,
	ThreadsWidget*	widget
	)
	:
	GetThreadsCmd(JString::empty, widget),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetThreads::~LLDBGetThreads()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex threadIDPattern = "^thread #([[:digit:]]+):\\s*tid = [^,]+,";

void
LLDBGetThreads::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<LLDBLink*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	lldb::SBProcess p = link->GetDebugger()->GetSelectedTarget().GetProcess();
	if (!p.IsValid())
	{
		return;
	}

	JTreeNode* root   = itsTree->GetRoot();
	const JSize count = p.GetNumThreads();
	JString fileName, name, indexStr;
	for (JUnsignedOffset i=0; i<count; i++)
	{
		lldb::SBThread t = p.GetThreadAtIndex(i);
		lldb::SBFrame f  = t.GetSelectedFrame();

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

		lldb::SBStream stream;
		t.GetDescription(stream);
		name = stream.GetData();

		const JStringMatch m = threadIDPattern.Match(name, JRegex::kIncludeSubmatches);
		if (!m.IsEmpty())
		{
			indexStr = m.GetSubstring(1);
			while (indexStr.GetCharacterCount() < kThreadIndexWidth)
			{
				indexStr.Prepend("0");
			}
			indexStr += ":  ";

			JStringIterator iter(&name);
			iter.RemoveNext(m.GetCharacterRange().last);
			name.TrimWhitespace();
			name.Prepend(indexStr);
		}

		auto* node = jnew ThreadNode(t.GetThreadID(), name, fileName, lineIndex);
		assert( node != nullptr );

		root->Append(node);
	}

	GetWidget()->FinishedLoading(p.GetSelectedThread().GetThreadID());
}
