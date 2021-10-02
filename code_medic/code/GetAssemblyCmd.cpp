/******************************************************************************
 GetAssemblyCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GetAssemblyCmd.h"
#include "SourceDirector.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetAssemblyCmd::GetAssemblyCmd
	(
	SourceDirector*	dir,
	const JString&		cmd
	)
	:
	Command(cmd, false, true),
	itsDir(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetAssemblyCmd::~GetAssemblyCmd()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
GetAssemblyCmd::HandleFailure()
{
	JPtrArray<JString> addrList(JPtrArrayT::kDeleteAll);
	itsDir->DisplayDisassembly(&addrList, JString::empty);
}
