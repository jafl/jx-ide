/******************************************************************************
 XDDisplaySourceForMain.cpp

	BASE CLASS = DisplaySourceForMainCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "XDDisplaySourceForMain.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDDisplaySourceForMain::XDDisplaySourceForMain
	(
	SourceDirector* sourceDir
	)
	:
	DisplaySourceForMainCmd(sourceDir, JString("status", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDDisplaySourceForMain::~XDDisplaySourceForMain()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDDisplaySourceForMain::HandleSuccess
	(
	const JString& data
	)
{
}
