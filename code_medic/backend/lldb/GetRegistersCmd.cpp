/******************************************************************************
 GetRegistersCmd.cpp

	BASE CLASS = GetRegistersCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/GetRegistersCmd.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "RegistersDir.h"
#include "lldb/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetRegistersCmd::GetRegistersCmd
	(
	RegistersDir* dir
	)
	:
	::GetRegistersCmd(JString::empty, dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetRegistersCmd::~GetRegistersCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
lldb::GetRegistersCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	SBCommandInterpreter interp = link->GetDebugger()->GetCommandInterpreter();
	if (!interp.IsValid())
	{
		return;
	}

	SBCommandReturnObject result;
	interp.HandleCommand("register read", result);

	// https://llvm.org/bugs/show_bug.cgi?id=26421
	if (result.IsValid() && result.Succeeded() /* && result.HasResult() */)
	{
		GetDirector()->Update(JString(result.GetOutput(), JString::kNoCopy));
	}
}
