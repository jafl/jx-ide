/******************************************************************************
 Breakpoint.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "Breakpoint.h"
#include "BreakpointManager.h"
#include "GetFullPathCmd.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

Breakpoint::Breakpoint
	(
	const JSize			debuggerIndex,
	const Location&	location,
	const JString&		fn,
	const JString&		addr,
	const bool		enabled,
	const Action		action,
	const JString&		condition,
	const JSize			ignoreCount
	)
	:
	itsDebuggerIndex(debuggerIndex),
	itsLocation(location),
	itsFnName(fn),
	itsAddr(addr),
	itsEnabledFlag(enabled),
	itsAction(action),
	itsCondition(condition),
	itsIgnoreCount(ignoreCount)
{
	BreakpointX();
}

Breakpoint::Breakpoint
	(
	const JSize		debuggerIndex,
	const JString&	fileName,
	const JSize		lineIndex,
	const JString&	fn,
	const JString&	addr,
	const bool	enabled,
	const Action	action,
	const JString&	condition,
	const JSize		ignoreCount
	)
	:
	itsDebuggerIndex(debuggerIndex),
	itsLocation(fileName, lineIndex),
	itsFnName(fn),
	itsAddr(addr),
	itsEnabledFlag(enabled),
	itsAction(action),
	itsCondition(condition),
	itsIgnoreCount(ignoreCount)
{
	BreakpointX();
}

// search target

Breakpoint::Breakpoint
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
	:
	itsDebuggerIndex(0),
	itsLocation(fileName, lineIndex),
	itsEnabledFlag(true),
	itsAction(kKeepBreakpoint),
	itsIgnoreCount(0)
{
	BreakpointX();
}

Breakpoint::Breakpoint
	(
	const JString& addr
	)
	:
	itsDebuggerIndex(0),
	itsAddr(addr),
	itsEnabledFlag(true),
	itsAction(kKeepBreakpoint),
	itsIgnoreCount(0)
{
	BreakpointX();
}

// private

void
Breakpoint::BreakpointX()
{
	const JString& fileName = GetFileName();
	if (!fileName.IsEmpty())	// search target may have empty file name
	{
		bool exists;
		JString fullName;
		if (GetLink()->FindFile(fileName, &exists, &fullName))
		{
			if (exists)
			{
				itsLocation.SetFileName(fullName);
			}
		}
		else
		{
			GetFullPathCmd* cmd = GetLink()->CreateGetFullPathCmd(fileName);
			ListenTo(cmd);
		}
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Breakpoint::~Breakpoint()
{
}

/******************************************************************************
 DisplayStatus

 *****************************************************************************/

void
Breakpoint::DisplayStatus()
	const
{
	GetLink()->ShowBreakpointInfo(itsDebuggerIndex);
}

/******************************************************************************
 ToggleEnabled

 *****************************************************************************/

void
Breakpoint::ToggleEnabled()
{
	GetLink()->SetBreakpointEnabled(itsDebuggerIndex, !itsEnabledFlag);
}

/******************************************************************************
 RemoveCondition

 *****************************************************************************/

void
Breakpoint::RemoveCondition()
{
	GetLink()->RemoveBreakpointCondition(itsDebuggerIndex);
}

/******************************************************************************
 SetIgnoreCount

 *****************************************************************************/

void
Breakpoint::SetIgnoreCount
	(
	const JSize count
	)
{
	GetLink()->SetBreakpointIgnoreCount(itsDebuggerIndex, count);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
Breakpoint::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(GetFullPathCmd::kFileFound))
	{
		auto* info = dynamic_cast<const GetFullPathCmd::FileFound*>(&message);
		assert( info != nullptr );
		itsLocation.SetFileName(info->GetFullName());
		GetLink()->GetBreakpointManager()->BreakpointFileNameResolved(this);
	}
	else if (message.Is(GetFullPathCmd::kFileNotFound))
	{
		GetLink()->GetBreakpointManager()->BreakpointFileNameInvalid(this);
	}
	else if (message.Is(GetFullPathCmd::kNewCommand))
	{
		auto* info = dynamic_cast<const GetFullPathCmd::NewCommand*>(&message);
		assert( info != nullptr );
		ListenTo(info->GetNewCommand());
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}
