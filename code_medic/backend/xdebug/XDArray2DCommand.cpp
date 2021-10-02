/******************************************************************************
 XDArray2DCommand.cpp

	BASE CLASS = Array2DCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDArray2DCommand.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDArray2DCommand::XDArray2DCommand
	(
	Array2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	Array2DCmd(dir, table, data)
{
	SetCommand("status");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDArray2DCommand::~XDArray2DCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDArray2DCommand::HandleSuccess
	(
	const JString& data
	)
{
}
