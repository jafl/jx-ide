/******************************************************************************
 JVMDisplaySourceForMain.cpp

	BASE CLASS = DisplaySourceForMainCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMDisplaySourceForMain.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMDisplaySourceForMain::JVMDisplaySourceForMain
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

JVMDisplaySourceForMain::~JVMDisplaySourceForMain()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMDisplaySourceForMain::HandleSuccess
	(
	const JString& data
	)
{
}
