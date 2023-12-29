/******************************************************************************
 GetThreadsCmd.cpp

	BASE CLASS = GetThreadsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/GetThreadsCmd.h"
#include "ThreadsWidget.h"
#include "jvm/ThreadNode.h"
#include "jvm/Link.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetThreadsCmd::GetThreadsCmd
	(
	JTree*			tree,
	ThreadsWidget*	widget
	)
	:
	::GetThreadsCmd("NOP", widget),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetThreadsCmd::~GetThreadsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetThreadsCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	CopyTree(link->GetThreadRoot(), itsTree->GetRoot());

	GetWidget()->FinishedLoading(link->GetCurrentThreadID());
}

/******************************************************************************
 CopyTree (private)

 ******************************************************************************/

void
jvm::GetThreadsCmd::CopyTree
	(
	ThreadNode*	src,
	JTreeNode*	dest
	)
{
	const JSize count = src->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto* child1 = dynamic_cast<ThreadNode*>(src->GetChild(i));

		auto* child2 = jnew ThreadNode(*child1);

		if (dest->IsRoot())
		{
			dest->Append(child2);
		}
		else
		{
			dynamic_cast<ThreadNode*>(dest)->AppendThread(child2);
		}

		CopyTree(child1, child2);
	}
}
