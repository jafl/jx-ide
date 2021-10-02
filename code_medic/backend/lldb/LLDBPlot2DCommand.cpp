/******************************************************************************
 LLDBPlot2DCommand.cpp

	BASE CLASS = Plot2DCommand

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBPlot2DCommand.h"
#include "Plot2DDir.h"
#include "LLDBLink.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBValue.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBPlot2DCommand::LLDBPlot2DCommand
	(
	Plot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
	:
	Plot2DCommand(dir, x, y)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBPlot2DCommand::~LLDBPlot2DCommand()
{
}

/******************************************************************************
 UpdateRange (virtual)

 ******************************************************************************/

void
LLDBPlot2DCommand::UpdateRange
	(
	const JIndex	curveIndex,
	const JInteger	min,
	const JInteger	max
	)
{
	Plot2DCommand::UpdateRange(curveIndex, min, max);

	itsCurveIndex = curveIndex;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex prefixPattern = "^\\$[[:digit:]]+[[:space:]]+=[[:space:]]+(.*)$";

void
LLDBPlot2DCommand::HandleSuccess
	(
	const JString& data
	)
{
	lldb::SBFrame f =
		dynamic_cast<LLDBLink*>(GetLink())->GetDebugger()->
			GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();

	JArray<JFloat>* x = GetX();
	JArray<JFloat>* y = GetY();

	if (!f.IsValid())
	{
		x->RemoveAll();
		y->RemoveAll();
		return;
	}

	const JIndex count = x->GetElementCount();
	JIndex i;
	JString expr, v;
	for (i=1; i<=count; i++)
	{
		expr             = GetDirector()->GetXExpression(itsCurveIndex, i);
		lldb::SBValue vx = f.EvaluateExpression(expr.GetBytes());
		JFloat x1;
		if (!JString::ConvertToFloat(vx.GetValue(), &x1))
		{
			break;
		}

		expr             = GetDirector()->GetYExpression(itsCurveIndex, i);
		lldb::SBValue vy = f.EvaluateExpression(expr.GetBytes());
		JFloat y1;
		if (!JString::ConvertToFloat(vy.GetValue(), &y1))
		{
			break;
		}

		x->SetElement(i, x1);
		y->SetElement(i, y1);
	}

	if (i <= count)
	{
		const JSize delta = count - (i-1);
		x->RemoveNextElements(count - delta + 1, delta);
		y->RemoveNextElements(count - delta + 1, delta);
	}
}
