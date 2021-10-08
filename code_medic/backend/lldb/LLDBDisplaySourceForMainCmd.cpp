/******************************************************************************
 LLDBDisplaySourceForMainCmd.cpp

	Finds main() and displays it in the Current Source window.

	BASE CLASS = DisplaySourceForMainCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBDisplaySourceForMainCmd.h"
#include "SourceDirector.h"
#include "LLDBLink.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBSymbolContextList.h"
#include "lldb/API/SBSymbolContext.h"
#include "lldb/API/SBCompileUnit.h"
#include "lldb/API/SBFileSpec.h"
#include "globals.h"
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::DisplaySourceForMainCmd::DisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
	:
	::DisplaySourceForMainCmd(sourceDir, JString::empty)
{
	ListenTo(GetLink());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::DisplaySourceForMainCmd::~DisplaySourceForMainCmd()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
lldb::DisplaySourceForMainCmd::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetLink() && message.Is(::Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const ::Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			Command::Send();
		}
	}
	else
	{
		::DisplaySourceForMainCmd::Receive(sender, message);
	}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
lldb::DisplaySourceForMainCmd::HandleSuccess
	(
	const JString& data
	)
{
	SBTarget t = dynamic_cast<Link*>(GetLink())->GetDebugger()->GetSelectedTarget();
	bool found = false;
	if (t.IsValid())
	{
		SBSymbolContextList list = t.FindFunctions("main");
		if (list.IsValid() && list.GetSize() == 1)
		{
			SBSymbolContext ctx = list.GetContextAtIndex(0);
			SBCompileUnit unit  = ctx.GetCompileUnit();
			SBFileSpec file     = unit.GetFileSpec();
			if (unit.IsValid() && file.IsValid())
			{
				// sym.GetLineEntry() is not valid -- we need to search

				JIndex line = 1;

				const addr_t target = ctx.GetSymbol().GetStartAddress().GetFileAddress();
				const JSize count   = unit.GetNumLineEntries();
				for (JUnsignedOffset i=0; i<count; i++)
				{
					SBLineEntry e = unit.GetLineEntryAtIndex(i);
					if (e.GetStartAddress().GetFileAddress() == target)
					{
						line = e.GetLine();
						break;
					}
				}

				JString fullName = JCombinePathAndName(
					JString(file.GetDirectory(), JString::kNoCopy),
					JString(file.GetFilename(), JString::kNoCopy));

				GetSourceDir()->DisplayFile(fullName, line, false);
				found = true;
			}
		}
	}


	if (!found)
	{
		GetLink()->NotifyUser(JGetString("CannotFindMain::LLDBDisplaySourceForMainCmd"), true);
		GetLink()->Log("LLDBDisplaySourceForMainCmd failed to find 'main'");

		GetSourceDir()->ClearDisplay();
	}
}
