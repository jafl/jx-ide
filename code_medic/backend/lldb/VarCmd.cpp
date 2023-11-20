/******************************************************************************
 VarCmd.cpp

	BASE CLASS = VarCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/VarCmd.h"
#include "VarNode.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBValue.h"
#include "lldb/Link.h"
#include "lldb/VarNode.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::VarCmd::VarCmd
	(
	const JString& expr
	)
	:
	::VarCmd()
{
	itsExpr = expr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::VarCmd::~VarCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
lldb::VarCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link == nullptr)
	{
		Broadcast(ValueMessage(kValueFailed, nullptr));
		return;
	}

	SBFrame f = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (!f.IsValid())
	{
		Broadcast(ValueMessage(kValueFailed, nullptr));
		return;
	}

	SBValue v = f.EvaluateExpression(itsExpr.GetBytes(), eDynamicDontRunTarget);
	if (v.IsValid())
	{
		Broadcast(ValueMessage(kValueUpdated, VarNode::BuildTree(f, v)));
	}
	else
	{
		Broadcast(ValueMessage(kValueFailed, nullptr));
	}
}
