/******************************************************************************
 JVMArray2DCmd.cpp

	BASE CLASS = Array2DCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jvm/Array2DCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::Array2DCmd::Array2DCmd
	(
	Array2DDir*			dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	::Array2DCmd(dir, table, data)
{
	SetCommand("NOP");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::Array2DCmd::~Array2DCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::Array2DCmd::HandleSuccess
	(
	const JString& data
	)
{
}
