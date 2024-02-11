/******************************************************************************
 GetMemoryCmd.cpp

	BASE CLASS = GetMemoryCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/GetMemoryCmd.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "MemoryDir.h"
#include "lldb/Link.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetMemoryCmd::GetMemoryCmd
	(
	MemoryDir* dir
	)
	:
	::GetMemoryCmd(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetMemoryCmd::~GetMemoryCmd()
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
lldb::GetMemoryCmd::HandleSuccess
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

	MemoryDir::DisplayType type;
	JSize count;
	const JString& expr = GetDirector()->GetExpression(&type, &count);

	JString cmd("memory read -g ");
	cmd += JString(count);
	cmd += kCommandName[ type-1 ];
	cmd += " ";
	cmd += expr;

	SBCommandReturnObject result;
	interp.HandleCommand(cmd.GetBytes(), result);

	if (result.IsValid() && result.Succeeded() && result.HasResult())
	{
		GetDirector()->Update(JString(result.GetOutput(), JString::kNoCopy));
	}
}
