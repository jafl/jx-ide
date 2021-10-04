/******************************************************************************
 LLDBGetThread.cpp

	BASE CLASS = GetThreadCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetThread.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "ThreadsWidget.h"
#include "LLDBGetThreads.h"
#include "globals.h"
#include "LLDBLink.h"	// must be after X11 includes: Status
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetThread::LLDBGetThread
	(
	ThreadsWidget* widget
	)
	:
	GetThreadCmd(JString::empty, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetThread::~LLDBGetThread()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetThread::HandleSuccess
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

	GetWidget()->FinishedLoading(p.GetSelectedThread().GetThreadID());
}
