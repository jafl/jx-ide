/******************************************************************************
 LLDBGetMemory.cpp

	BASE CLASS = GetMemoryCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetMemory.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "MemoryDir.h"
#include "LLDBLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetMemory::LLDBGetMemory
	(
	MemoryDir* dir
	)
	:
	GetMemoryCmd(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetMemory::~LLDBGetMemory()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JUtf8Byte* kCommandName[] =
{
	"xb", "xh", "xw", "xg", "cb", "i"
};

void
LLDBGetMemory::HandleSuccess
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

	MemoryDir::DisplayType type;
	JSize count;
	const JString& expr = GetDirector()->GetExpression(&type, &count);

	JString cmd("memory read -g ");
	cmd += JString((JUInt64) count);
	cmd += kCommandName[ type-1 ];
	cmd += " ";
	cmd += expr;

	lldb::SBCommandReturnObject result;
	interp.HandleCommand(cmd.GetBytes(), result);

	if (result.IsValid() && result.Succeeded() && result.HasResult())
	{
		GetDirector()->Update(JString(result.GetOutput(), JString::kNoCopy));
	}
}
