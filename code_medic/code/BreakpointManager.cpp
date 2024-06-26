/******************************************************************************
 BreakpointManager.cpp

	BASE CLASS = virtual public JBroadcaster

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "BreakpointManager.h"
#include "GetBreakpointsCmd.h"
#include "globals.h"
#include "fileVersions.h"
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* BreakpointManager::kBreakpointsChanged =
	"BreakpointsChanged::BreakpointManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

BreakpointManager::BreakpointManager
	(
	Link*				link,
	GetBreakpointsCmd*	cmd
	)
	:
	itsLink(link),
	itsCmd(cmd),
	itsSavedBPList(nullptr),
	itsRestoreBreakpointsFlag(false),
	itsUpdateWhenStopFlag(false)
{
	assert( itsCmd != nullptr );

	itsBPList = jnew JPtrArray<Breakpoint>(JPtrArrayT::kDeleteAll);
	itsBPList->SetCompareFunction(CompareBreakpointLocations);
	itsBPList->SetSortOrder(JListT::kSortAscending);

	itsOtherList = jnew JPtrArray<Breakpoint>(JPtrArrayT::kDeleteAll);

	ListenTo(itsLink);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

BreakpointManager::~BreakpointManager()
{
	jdelete itsCmd;
	jdelete itsBPList;
	jdelete itsSavedBPList;
	jdelete itsOtherList;
}

/******************************************************************************
 HasBreakpointAt

 *****************************************************************************/

bool
BreakpointManager::HasBreakpointAt
	(
	const Location& loc
	)
	const
{
	Breakpoint target(loc.GetFileName(), loc.GetLineNumber());
	JIndex i;
	return itsBPList->SearchSorted(&target, JListT::kAnyMatch, &i);
}

/******************************************************************************
 GetBreakpoints

	*** BreakpointManager owns the objects returned in list!

 *****************************************************************************/

bool
BreakpointManager::GetBreakpoints
	(
	const JString&			fileName,
	JPtrArray<Breakpoint>*	list
	)
	const
{
	list->RemoveAll();
	list->SetCleanUpAction(JPtrArrayT::kForgetAll);

	if (JIsAbsolutePath(fileName) && JFileExists(fileName))
	{
		Breakpoint target(fileName, 1);
		bool found;
		const JIndex startIndex =
			itsBPList->SearchSortedOTI(&target, JListT::kFirstMatch, &found);

		const JSize count = itsBPList->GetItemCount();
		for (JIndex i=startIndex; i<=count; i++)
		{
			Breakpoint* bp = itsBPList->GetItem(i);
			if (bp->GetFileID() == target.GetFileID())
			{
				list->Append(bp);
			}
			else
			{
				break;
			}
		}
	}

	return !list->IsEmpty();
}

/******************************************************************************
 GetBreakpoints

	*** BreakpointManager owns the objects returned in list!

 *****************************************************************************/

bool
BreakpointManager::GetBreakpoints
	(
	const Location&			loc,
	JPtrArray<Breakpoint>*	list
	)
	const
{
	list->RemoveAll();
	list->SetCleanUpAction(JPtrArrayT::kForgetAll);

	if (loc.GetFileID().IsValid())
	{
		Breakpoint target(loc.GetFileName(), loc.GetLineNumber());
		bool found;
		const JIndex startIndex =
			itsBPList->SearchSortedOTI(&target, JListT::kFirstMatch, &found);

		const JSize count = itsBPList->GetItemCount();
		for (JIndex i=startIndex; i<=count; i++)
		{
			Breakpoint* bp = itsBPList->GetItem(i);
			if (bp->GetLocation() == loc)
			{
				list->Append(bp);
			}
			else
			{
				break;
			}
		}
	}
	else if (!loc.GetFunctionName().IsEmpty())
	{
		const JString fn = loc.GetFunctionName() + "(";

		const JSize count = itsBPList->GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			Breakpoint* bp = itsBPList->GetItem(i);
			if (bp->GetFunctionName() == loc.GetFunctionName() ||
				bp->GetFunctionName().StartsWith(fn))
			{
				list->Append(bp);
			}
		}
	}

	return !list->IsEmpty();
}

/******************************************************************************
 EnableAll

 ******************************************************************************/

void
BreakpointManager::EnableAll()
{
	for (auto* bp : *itsBPList)
	{
		if (!bp->IsEnabled())
		{
			itsLink->SetBreakpointEnabled(bp->GetDebuggerIndex(), true);
		}
	}
}

/******************************************************************************
 DisableAll

 ******************************************************************************/

void
BreakpointManager::DisableAll()
{
	for (auto* bp : *itsBPList)
	{
		if (bp->IsEnabled())
		{
			itsLink->SetBreakpointEnabled(bp->GetDebuggerIndex(), false);
		}
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
BreakpointManager::ReadSetup
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	itsLink->RemoveAllBreakpoints();

	JSize count;
	input >> count;

	if (count > 0)
	{
		jdelete itsSavedBPList;
		itsSavedBPList = jnew JPtrArray<Breakpoint>(JPtrArrayT::kDeleteAll);
	}

	JString fileName, condition, commands;
	JIndex lineNumber;
	bool enabled, hasCondition, hasCommands;
	JSize ignoreCount;
	Breakpoint::Action action;
	long tempAction;
	for (JIndex i=1; i<=count; i++)
	{
		input >> fileName >> lineNumber;
		input >> JBoolFromString(enabled) >> tempAction >> ignoreCount;
		input >> JBoolFromString(hasCondition) >> condition;

		if (vers == 1)
		{
			input >> JBoolFromString(hasCommands) >> commands;
		}

		action = (Breakpoint::Action) tempAction;
		if (!hasCondition)
		{
			condition.Clear();
		}

		auto* bp = jnew Breakpoint(0, fileName, lineNumber,
											JString::empty, JString::empty,
											enabled, action, condition,
											ignoreCount);
		itsSavedBPList->Append(bp);

		// set breakpoint after saving, so name resolution happens first

		itsLink->SetBreakpoint(fileName, lineNumber, action == Breakpoint::kRemoveBreakpoint);
	}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
BreakpointManager::WriteSetup
	(
	std::ostream& output
	)
	const
{
	const JSize count = itsBPList->GetItemCount();
	output << ' ' << count;

	JString s;
	for (JIndex i=1; i<=count; i++)
	{
		Breakpoint* bp = itsBPList->GetItem(i);
		output << ' ' << bp->GetFileName();
		output << ' ' << bp->GetLineNumber();
		output << ' ' << JBoolToString(bp->IsEnabled());
		output << ' ' << (long) bp->GetAction();
		output << ' ' << bp->GetIgnoreCount();
		output << ' ' << JBoolToString(bp->GetCondition(&s));
		output << ' ' << s;
	}
}

/******************************************************************************
 Receive (virtual protected)

	We restore the breakpoints if the debugger is restarted.

 ******************************************************************************/

void
BreakpointManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(Link::kDebuggerRestarted))
	{
		itsRestoreBreakpointsFlag = true;
	}
	else if (sender == itsLink && message.Is(Link::kSymbolsLoaded))
	{
		auto& info        = dynamic_cast<const Link::SymbolsLoaded&>(message);
		const JSize count = itsBPList->GetItemCount();
		if (info.Successful() && itsRestoreBreakpointsFlag && count > 0)
		{
			jdelete itsSavedBPList;
			itsSavedBPList = jnew JPtrArray<Breakpoint>(JPtrArrayT::kDeleteAll);
			itsSavedBPList->CopyObjects(*itsBPList, JPtrArrayT::kDeleteAll, false);

			for (JIndex i=1; i<=count; i++)
			{
				itsLink->SetBreakpoint(*itsBPList->GetItem(i));
			}
		}
		itsRestoreBreakpointsFlag = false;
	}

	else if (sender == itsLink && message.Is(Link::kBreakpointsChanged))
	{
		itsCmd->Send();
	}
	else if (sender == itsLink && message.Is(Link::kProgramStopped))
	{
		if (itsUpdateWhenStopFlag)
		{
			itsCmd->Send();
		}
	}

	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateBreakpoints

 *****************************************************************************/

void
BreakpointManager::UpdateBreakpoints
	(
	const JPtrArray<Breakpoint>& bpList,
	const JPtrArray<Breakpoint>& otherList
	)
{
	itsBPList->CopyPointers(bpList, JPtrArrayT::kDeleteAll, false);
	itsOtherList->CopyPointers(otherList, JPtrArrayT::kDeleteAll, false);
	Broadcast(BreakpointsChanged());

	if (itsSavedBPList != nullptr &&
		itsSavedBPList->GetItemCount() == itsBPList->GetItemCount())
	{
		const JSize count = itsSavedBPList->GetItemCount();
		JString condition;
		for (JIndex i=1; i<=count; i++)
		{
			Breakpoint* bp = itsSavedBPList->GetItem(i);
			const JIndex j   = (itsBPList->GetItem(i))->GetDebuggerIndex();

			if (!bp->IsEnabled())
			{
				itsLink->SetBreakpointEnabled(j, false);
			}
			else if (bp->GetAction() == Breakpoint::kDisableBreakpoint)
			{
				itsLink->SetBreakpointEnabled(j, true, true);
			}

			if (bp->GetCondition(&condition))
			{
				itsLink->SetBreakpointCondition(j, condition);
			}

			if (bp->GetIgnoreCount() > 0)
			{
				itsLink->SetBreakpointIgnoreCount(j, bp->GetIgnoreCount());
			}
		}

		jdelete itsSavedBPList;
		itsSavedBPList = nullptr;
	}
}

/******************************************************************************
 BreakpointFileNameResolved

 *****************************************************************************/

void
BreakpointManager::BreakpointFileNameResolved
	(
	Breakpoint* bp
	)
{
	if (itsBPList->Includes(bp))
	{
		itsBPList->Remove(bp);
		itsBPList->InsertSorted(bp);
		Broadcast(BreakpointsChanged());
	}
}

/******************************************************************************
 BreakpointFileNameInvalid

 *****************************************************************************/

void
BreakpointManager::BreakpointFileNameInvalid
	(
	Breakpoint* bp
	)
{
	if (itsSavedBPList != nullptr)
	{
		itsSavedBPList->Remove(bp);
	}
}

/******************************************************************************
 CompareBreakpointLocations (static)

 ******************************************************************************/

std::weak_ordering
BreakpointManager::CompareBreakpointLocations
	(
	Breakpoint* const & bp1,
	Breakpoint* const & bp2
	)
{
	auto r = JFileID::Compare(bp1->GetFileID(), bp2->GetFileID());
	if (r == std::weak_ordering::equivalent)
	{
		r = bp1->GetLineNumber() <=> bp2->GetLineNumber();
	}
	return r;
}
