/******************************************************************************
 JVMArray2DCommand.cpp

	BASE CLASS = Array2DCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMArray2DCommand.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMArray2DCommand::JVMArray2DCommand
	(
	Array2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	Array2DCmd(dir, table, data)
{
	SetCommand("NOP");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMArray2DCommand::~JVMArray2DCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMArray2DCommand::HandleSuccess
	(
	const JString& data
	)
{
}
