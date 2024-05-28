/******************************************************************************
 GetThreadsCmd.cpp

	BASE CLASS = GetThreadsCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/GetThreadsCmd.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBStream.h"
#include "ThreadsWidget.h"
#include "ThreadNode.h"
#include "globals.h"
#include "lldb/Link.h"	// must be after X11 includes: Status
#include <jx-af/jx/JXFunctionTask.h>
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kThreadIndexWidth = 2;	// width of thread index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetThreadsCmd::GetThreadsCmd
	(
	JTree*			tree,
	ThreadsWidget*	widget
	)
	:
	::GetThreadsCmd(JString::empty, widget),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetThreadsCmd::~GetThreadsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex threadIDPattern("^thread #([[:digit:]]+):\\s*tid = [^,]+,");

void
lldb::GetThreadsCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	SBProcess p = link->GetDebugger()->GetSelectedTarget().GetProcess();
	if (!p.IsValid())
	{
		return;
	}

	JIndex threadID = GetWidget()->GetLastSelectedThread();

	JTreeNode* root   = itsTree->GetRoot();
	const JSize count = p.GetNumThreads();
	JString fileName, name, indexStr;
	bool foundThreadID = false;
	for (JUnsignedOffset i=0; i<count; i++)
	{
		SBThread t = p.GetThreadAtIndex(i);
		SBFrame f  = t.GetSelectedFrame();

		JIndex lineIndex = 0;
		SBLineEntry e    = f.GetLineEntry();
		SBFileSpec file  = e.GetFileSpec();
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

		SBStream stream;
		t.GetDescription(stream);
		name = stream.GetData();

		const JStringMatch m = threadIDPattern.Match(name, JRegex::kIncludeSubmatches);
		if (!m.IsEmpty())
		{
			indexStr = m.GetSubstring(1);

			JUInt indexValue;
			if (indexStr.ConvertToUInt(&indexValue) && indexValue == threadID)
			{
				foundThreadID = true;
			}

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

		root->Append(jnew ThreadNode(t.GetThreadID(), name, fileName, lineIndex));
	}

	if (!foundThreadID)
	{
		threadID = 0;
	}

	GetWidget()->FinishedLoading(0);

	if (threadID > 0 && threadID != p.GetSelectedThread().GetThreadID())
	{
		auto* task = jnew JXFunctionTask(50, [this, threadID]()
		{
			GetWidget()->SelectThread(threadID);
		},
		"lldb::GetThreadsCmd::SelectThread",
		true);
		task->Start();
	}
	else
	{
		GetWidget()->SelectThread(threadID > 0 ? threadID : p.GetSelectedThread().GetThreadID());
	}
}
