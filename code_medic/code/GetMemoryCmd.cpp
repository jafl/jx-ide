/******************************************************************************
 GetMemoryCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GetMemoryCmd.h"
#include "MemoryDir.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetMemoryCmd::GetMemoryCmd
	(
	MemoryDir* dir
	)
	:
	Command("", false, true),
	itsDir(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetMemoryCmd::~GetMemoryCmd()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetMemoryCmd::HandleFailure()
{
	itsDir->Update(JString::empty);
}
