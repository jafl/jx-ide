/******************************************************************************
 GetSourceFileListCmd.cpp

	BASE CLASS = Command, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GetSourceFileListCmd.h"
#include "FileListDir.h"
#include "globals.h"
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GetSourceFileListCmd::GetSourceFileListCmd
	(
	const JString&	cmd,
	FileListDir*	fileList
	)
	:
	Command(cmd, false, false),
	itsFileList(fileList),
	itsNeedRedoOnFirstStop(true)
{
	ListenTo(GetLink());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetSourceFileListCmd::~GetSourceFileListCmd()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GetSourceFileListCmd::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	Link* link = GetLink();

	if (sender == link && message.Is(Link::kPrepareToLoadSymbols))
	{
		(itsFileList->GetTable())->RemoveAllFiles();
	}
	else if (sender == link && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			Command::Send();
			itsNeedRedoOnFirstStop = true;
		}
	}
	else if (sender == link && message.Is(Link::kSymbolsReloaded))
	{
		Command::Send();
	}
	else if (sender == link && message.Is(Link::kProgramFirstStop))
	{
		if (itsNeedRedoOnFirstStop)
		{
			Command::Send();
			itsNeedRedoOnFirstStop = false;
		}
	}
	else if (sender == link &&
			 (message.Is(Link::kCoreLoaded) ||
			  message.Is(Link::kAttachedToProcess)))
	{
		Command::Send();
	}

	else
	{
		JBroadcaster::Receive(sender, message);
	}
}
