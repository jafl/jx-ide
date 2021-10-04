/******************************************************************************
 JVMArray2DCmd.cpp

	BASE CLASS = Array2DCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMArray2DCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMArray2DCmd::JVMArray2DCmd
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

JVMArray2DCmd::~JVMArray2DCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMArray2DCmd::HandleSuccess
	(
	const JString& data
	)
{
}
