/******************************************************************************
 GDBDisplaySourceForMainCmd.cpp

	Finds main() and displays it in the Current Source window.

	BASE CLASS = DisplaySourceForMainCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBDisplaySourceForMainCmd.h"
#include "SourceDirector.h"
#include "GDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kCommand[] =
{
	"info line main",
	"info line _start",
	"info line MAIN__"
};

static const JUtf8Byte* kBreakCommand[] =
{
	"tbreak main",
	"tbreak _start",
	"tbreak MAIN__"
};

const JSize kCommandCount = sizeof(kCommand) / sizeof(JUtf8Byte*);

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::DisplaySourceForMainCmd::DisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
	:
	::DisplaySourceForMainCmd(sourceDir, JString(kCommand[0], JString::kNoCopy)),
	itsHasCoreFlag(false),
	itsNextCmdIndex(1)
{
	ListenTo(GetLink());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::DisplaySourceForMainCmd::~DisplaySourceForMainCmd()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
gdb::DisplaySourceForMainCmd::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetLink() && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		itsHasCoreFlag = GetLink()->HasCore();
		if (info->Successful())
		{
			itsNextCmdIndex = 1;
			SetCommand(kCommand[0]);
			Command::Send();
		}
		else if (!itsHasCoreFlag)
		{
			GetSourceDir()->ClearDisplay();
		}
	}
	else
	{
		DisplaySourceForMainCmd::Receive(sender, message);
	}
}

/******************************************************************************
 HandleSuccess (virtual protected)

	Look for the special format provided by using --fullname option.

 ******************************************************************************/

static const JRegex infoPattern =
	"Line [[:digit:]]+ of \"[^\"]*\" starts at address";
static const JRegex locationPattern =
	"032032(.+):([[:digit:]]+):[[:digit:]]+:[^:]+:0x[[:xdigit:]]+";

void
gdb::DisplaySourceForMainCmd::HandleSuccess
	(
	const JString& data
	)
{
	if (infoPattern.Match(data))
	{
		const JStringMatch m = locationPattern.Match(data, JRegex::kIncludeSubmatches);
		if (!m.IsEmpty())
		{
			JIndex lineIndex;
			const bool ok = m.GetSubstring(2).ConvertToUInt(&lineIndex);
			assert( ok );

			if (!itsHasCoreFlag)
			{
				GetSourceDir()->DisplayFile(m.GetSubstring(1), lineIndex, false);
			}
		}

		const JUtf8Byte* map[] =
		{
			"tbreak_cmd", kBreakCommand[ itsNextCmdIndex-1 ]
		};
		const JString cmd = JGetString("RunCommand::GDBDisplaySourceForMainCmd", map, sizeof(map));
		dynamic_cast<Link*>(GetLink())->SendWhenStopped(cmd);
	}
	else if (itsNextCmdIndex < kCommandCount)
	{
		SetCommand(kCommand [ itsNextCmdIndex ]);
		itsNextCmdIndex++;
		Command::Send();
	}
	else
	{
		GetLink()->Log("GDBDisplaySourceForMainCmd failed to match");

		if (!itsHasCoreFlag)
		{
			GetSourceDir()->ClearDisplay();
		}

		dynamic_cast<Link*>(GetLink())->FirstBreakImpossible();

		const JUtf8Byte* map[] =
		{
			"tbreak_cmd", ""
		};
		const JString cmd = JGetString("RunCommand::GDBDisplaySourceForMainCmd", map, sizeof(map));
		dynamic_cast<Link*>(GetLink())->SendWhenStopped(cmd);
	}
}
