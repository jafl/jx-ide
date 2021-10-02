/******************************************************************************
 JVMGetThreads.cpp

	BASE CLASS = GetThreads

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetThreads.h"
#include "ThreadsWidget.h"
#include "JVMThreadNode.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThreads::JVMGetThreads
	(
	JTree*				tree,
	ThreadsWidget*	widget
	)
	:
	GetThreads(JString("NOP", JString::kNoCopy), widget),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThreads::~JVMGetThreads()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThreads::HandleSuccess
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
JVMGetThreads::CopyTree
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
