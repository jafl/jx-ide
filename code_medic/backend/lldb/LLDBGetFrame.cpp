/******************************************************************************
 LLDBGetFrame.cpp

	BASE CLASS = GetFrameCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetFrame.h"
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

LLDBGetFrame::LLDBGetFrame
	(
	StackWidget* widget
	)
	:
	GetFrameCmd(JString::empty),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetFrame::~LLDBGetFrame()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetFrame::HandleSuccess
	(
	const JString& cmdData
	)
{
	auto* link = dynamic_cast<LLDBLink*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	lldb::SBFrame f = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
	{
		itsWidget->SelectFrame(f.GetFrameID());
	}
}
