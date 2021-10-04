/******************************************************************************
 JVMGetThreadsCmd.cpp

	BASE CLASS = GetThreadsCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetThreadsCmd.h"
#include "ThreadsWidget.h"
#include "JVMThreadNode.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThreadsCmd::JVMGetThreadsCmd
	(
	JTree*				tree,
	ThreadsWidget*	widget
	)
	:
	GetThreadsCmd(JString("NOP", JString::kNoCopy), widget),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThreadsCmd::~JVMGetThreadsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThreadsCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<JVMLink*>(GetLink());
	CopyTree(link->GetThreadRoot(), itsTree->GetRoot());

	GetWidget()->FinishedLoading(link->GetCurrentThreadID());
}

/******************************************************************************
 CopyTree (private)

 ******************************************************************************/

void
JVMGetThreadsCmd::CopyTree
	(
	JVMThreadNode*	src,
	JTreeNode*		dest
	)
{
	const JSize count = src->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto* child1 = dynamic_cast<JVMThreadNode*>(src->GetChild(i));

		auto* child2 = jnew JVMThreadNode(*child1);
		assert( child2 != nullptr );

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
