/******************************************************************************
 XDPlot2DCommand.cpp

	BASE CLASS = Plot2DCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDPlot2DCommand.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDPlot2DCommand::XDPlot2DCommand
	(
	Plot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
	:
	Plot2DCmd(dir, x, y)
{
	SetCommand("status");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDPlot2DCommand::~XDPlot2DCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDPlot2DCommand::HandleSuccess
	(
	const JString& data
	)
{
}
