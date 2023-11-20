/******************************************************************************
 GetRegistersCmd.cpp

	BASE CLASS = GetRegistersCmd

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "gdb/GetRegistersCmd.h"
#include "RegistersDir.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetRegistersCmd::GetRegistersCmd
	(
	RegistersDir* dir
	)
	:
	::GetRegistersCmd(JString("info all-registers", JString::kNoCopy), dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetRegistersCmd::~GetRegistersCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
gdb::GetRegistersCmd::HandleSuccess
	(
	const JString& data
	)
{
	GetDirector()->Update(data);
}
