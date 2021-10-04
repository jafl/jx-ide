/******************************************************************************
 GetStackCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetStackCmd.h"
#include "StackWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetStackCmd::GetStackCmd
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

GetStackCmd::~GetStackCmd()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetStackCmd::HandleFailure()
{
	itsWidget->FinishedLoading(0);
}
