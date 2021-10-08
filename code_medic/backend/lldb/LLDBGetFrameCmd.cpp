/******************************************************************************
 LLDBGetFrameCmd.cpp

	BASE CLASS = GetFrameCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetFrameCmd.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "StackWidget.h"
#include "globals.h"
#include "LLDBLink.h"	// must be after X11 includes: Status
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetFrameCmd::GetFrameCmd
	(
	StackWidget* widget
	)
	:
	::GetFrameCmd(JString::empty),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetFrameCmd::~GetFrameCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
lldb::GetFrameCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	SBFrame f = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
	{
		itsWidget->SelectFrame(f.GetFrameID());
	}
}
