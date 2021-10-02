/******************************************************************************
 LLDBGetRegisters.cpp

	BASE CLASS = GetRegisters

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetRegisters.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "RegistersDir.h"
#include "LLDBLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetRegisters::LLDBGetRegisters
	(
	RegistersDir* dir
	)
	:
	GetRegisters(JString::empty, dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetRegisters::~LLDBGetRegisters()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetRegisters::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<LLDBLink*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	lldb::SBCommandInterpreter interp = link->GetDebugger()->GetCommandInterpreter();
	if (!interp.IsValid())
	{
		return;
	}

	lldb::SBCommandReturnObject result;
	interp.HandleCommand("register read", result);

	// https://llvm.org/bugs/show_bug.cgi?id=26421
	if (result.IsValid() && result.Succeeded() /* && result.HasResult() */)
	{
		GetDirector()->Update(JString(result.GetOutput(), JString::kNoCopy));
	}
}
