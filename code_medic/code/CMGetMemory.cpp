/******************************************************************************
 CMGetMemory.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "CMGetMemory.h"
#include "CMMemoryDir.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetMemory::CMGetMemory
	(
	CMMemoryDir* dir
	)
	:
	CMCommand("", false, true),
	itsDir(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetMemory::~CMGetMemory()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
CMGetMemory::HandleFailure()
{
	itsDir->Update(JString::empty);
}
