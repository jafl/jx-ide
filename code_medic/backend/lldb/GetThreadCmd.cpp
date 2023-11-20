/******************************************************************************
 GetThreadCmd.cpp

	BASE CLASS = GetThreadCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/GetThreadCmd.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "ThreadsWidget.h"
#include "lldb/GetThreadsCmd.h"
#include "globals.h"
#include "lldb/Link.h"	// must be after X11 includes: Status
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetThreadCmd::GetThreadCmd
	(
	ThreadsWidget* widget
	)
	:
	::GetThreadCmd(JString::empty, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetThreadCmd::~GetThreadCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
lldb::GetThreadCmd::HandleSuccess
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

	GetWidget()->FinishedLoading(p.GetSelectedThread().GetThreadID());
}
