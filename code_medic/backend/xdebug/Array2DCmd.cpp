/******************************************************************************
 XDArray2DCmd.cpp

	BASE CLASS = Array2DCmd

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "xdebug/Array2DCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

xdebug::Array2DCmd::Array2DCmd
	(
	Array2DDir*			dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	::Array2DCmd(dir, table, data)
{
	SetCommand("status");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

xdebug::Array2DCmd::~Array2DCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
xdebug::Array2DCmd::HandleSuccess
	(
	const JString& data
	)
{
}
