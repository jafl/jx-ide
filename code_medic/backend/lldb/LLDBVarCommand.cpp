/******************************************************************************
 LLDBVarCommand.cpp

	BASE CLASS = VarCommand

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBVarCommand.h"
#include "VarNode.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBValue.h"
#include "LLDBLink.h"
#include "LLDBVarNode.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBVarCommand::LLDBVarCommand
	(
	const JString& expr
	)
	:
	VarCommand()
{
	itsExpr = expr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBVarCommand::~LLDBVarCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBVarCommand::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<LLDBLink*>(GetLink());
	if (link == nullptr)
	{
		Broadcast(ValueMessage(kValueFailed, nullptr));
		return;
	}

	lldb::SBFrame f = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (!f.IsValid())
	{
		Broadcast(ValueMessage(kValueFailed, nullptr));
		return;
	}

	lldb::SBValue v = f.EvaluateExpression(itsExpr.GetBytes(), lldb::eDynamicDontRunTarget);
	if (v.IsValid())
	{
		Broadcast(ValueMessage(kValueUpdated, LLDBVarNode::BuildTree(f, v)));
	}
	else
	{
		Broadcast(ValueMessage(kValueFailed, nullptr));
	}
}
