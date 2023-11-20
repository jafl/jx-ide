/******************************************************************************
 GetBreakpointsCmd.cpp

	BASE CLASS = GetBreakpointsCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "lldb/GetBreakpointsCmd.h"
#include "BreakpointManager.h"
#include "lldb/Link.h"
#include "lldb/API/SBBreakpoint.h"
#include "lldb/API/SBBreakpointLocation.h"
#include "lldb/API/SBAddress.h"
#include "lldb/API/SBFunction.h"
#include "lldb/API/SBLineEntry.h"
#include "lldb/API/SBFileSpec.h"
#include "globals.h"
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetBreakpointsCmd::GetBreakpointsCmd()
	:
	::GetBreakpointsCmd(JString::empty)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetBreakpointsCmd::~GetBreakpointsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

void
lldb::GetBreakpointsCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	SBTarget t = link->GetDebugger()->GetSelectedTarget();
	if (!t.IsValid())
	{
		return;
	}

	(GetLink()->GetBreakpointManager())->SetUpdateWhenStop(false);

	JPtrArray<Breakpoint> bpList(JPtrArrayT::kForgetAll);	// ownership taken by BreakpointManager
	bpList.SetCompareFunction(::BreakpointManager::CompareBreakpointLocations);
	bpList.SetSortOrder(JListT::kSortAscending);

	JPtrArray<Breakpoint> otherList(JPtrArrayT::kForgetAll);	// ownership taken by BreakpointManager

	JSize count = t.GetNumBreakpoints();
	JString func, addr, cond;
	for (JUnsignedOffset i=0; i<count; i++)
	{
		SBBreakpoint b = t.GetBreakpointAtIndex(i);
		if (!b.IsValid() || b.GetNumLocations() == 0)
		{
			continue;
		}

		SBBreakpointLocation loc = b.GetLocationAtIndex(0);
		SBAddress a              = loc.GetAddress();
		SBFunction fn            = a.GetFunction();
		SBLineEntry e            = a.GetLineEntry();
		SBFileSpec file          = e.GetFileSpec();

		const Breakpoint::Action action = b.IsOneShot() ?
			Breakpoint::kRemoveBreakpoint :
			Breakpoint::kKeepBreakpoint;

		if (fn.IsValid())
		{
			func = fn.GetName();
			addr = JString(a.GetLoadAddress(t), JString::kBase16);
		}
		else
		{
			func.Clear();
			addr.Clear();
		}

		if (b.GetCondition() != nullptr)
		{
			cond = b.GetCondition();
		}
		else
		{
			cond.Clear();
		}

		Breakpoint* bp = nullptr;
		if (file.IsValid())
		{
			const JString fullName = JCombinePathAndName(
				JString(file.GetDirectory(), JString::kNoCopy),
				JString(file.GetFilename(), JString::kNoCopy));

			bp = jnew Breakpoint(b.GetID(), fullName, e.GetLine(), func, addr,
								  b.IsEnabled(), action,
								  cond, b.GetIgnoreCount());
			assert( bp != nullptr );
		}
		else if (fn.IsValid())
		{
			Location loc;
			bp = jnew Breakpoint(b.GetID(), loc, func, addr,
								  b.IsEnabled(), action,
								  cond, b.GetIgnoreCount());
			assert( bp != nullptr );
		}

		if (bp != nullptr)
		{
			bpList.InsertSorted(bp);

			// may be deleted or other status change

			if (action != Breakpoint::kKeepBreakpoint || b.GetIgnoreCount() > 0)
			{
				(GetLink()->GetBreakpointManager())->SetUpdateWhenStop(true);
			}
		}
	}

	count = t.GetNumWatchpoints();
	if (count > 0)	// may be deleted when go out of scope
	{
		(GetLink()->GetBreakpointManager())->SetUpdateWhenStop(true);
	}

	for (JUnsignedOffset i=0; i<count; i++)
	{
		SBWatchpoint w = t.GetWatchpointAtIndex(i);

		Location loc;
		addr = JString(w.GetWatchAddress(), JString::kBase16);

		if (w.GetCondition() != nullptr)
		{
			cond = w.GetCondition();
		}
		else
		{
			cond.Clear();
		}

		auto* bp = jnew Breakpoint(w.GetID(), loc, JString::empty, addr,
								 w.IsEnabled(), Breakpoint::kKeepBreakpoint,
								 cond, w.GetIgnoreCount());
		assert( bp != nullptr );
		bpList.InsertSorted(bp);
	}

	GetLink()->GetBreakpointManager()->UpdateBreakpoints(bpList, otherList);
}
