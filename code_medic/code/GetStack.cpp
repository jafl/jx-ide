/******************************************************************************
 GetStack.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetStack.h"
#include "StackWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetStack::GetStack
	(
	const JString&	cmd,
	JTree*			tree,
	StackWidget*	widget
	)
	:
	Command(cmd, false, true),
	itsTree(tree),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetStack::~GetStack()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetStack::HandleFailure()
{
	itsWidget->FinishedLoading(0);
}
