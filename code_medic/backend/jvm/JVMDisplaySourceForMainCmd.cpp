/******************************************************************************
 JVMDisplaySourceForMainCmd.cpp

	BASE CLASS = DisplaySourceForMainCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMDisplaySourceForMainCmd.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMDisplaySourceForMainCmd::JVMDisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
	:
	DisplaySourceForMainCmd(sourceDir, JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMDisplaySourceForMainCmd::~JVMDisplaySourceForMainCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMDisplaySourceForMainCmd::HandleSuccess
	(
	const JString& data
	)
{
}
