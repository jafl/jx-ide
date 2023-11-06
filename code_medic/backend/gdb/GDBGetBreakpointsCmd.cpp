/******************************************************************************
 GDBGetBreakpointsCmd.cpp

	This is the only way to get all the relevant information about each
	breakpoint.  gdb does not print enough information when "break" is
	used.

	BASE CLASS = GetBreakpointsCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetBreakpointsCmd.h"
#include "BreakpointManager.h"
#include "GDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetBreakpointsCmd::GetBreakpointsCmd()
	:
	::GetBreakpointsCmd(JString("-break-list", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetBreakpointsCmd::~GetBreakpointsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

static const JRegex bpPattern = "\\bbkpt=\\{";

void
gdb::GetBreakpointsCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	(GetLink()->GetBreakpointManager())->SetUpdateWhenStop(false);

	JPtrArray<Breakpoint> bpList(JPtrArrayT::kForgetAll);		// ownership taken by BreakpointManager
	bpList.SetCompareFunction(::BreakpointManager::CompareBreakpointLocations);
	bpList.SetSortOrder(JListT::kSortAscending);

	JPtrArray<Breakpoint> otherList(JPtrArrayT::kForgetAll);	// ownership taken by BreakpointManager

	const JString& data = GetLastResult();
	std::istringstream stream(data.GetBytes());

	JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);

	JStringIterator iter(data);
	while (iter.Next(bpPattern))
	{
		stream.seekg(iter.GetLastMatch().GetUtf8ByteRange().last);
		if (!Link::ParseMap(stream, &map))
		{
			Link::Log("invalid data map");
			break;
		}
		iter.MoveTo(JStringIterator::kStartAfterChar, (std::streamoff) stream.tellg());

		JString* s;
		if (!map.GetElement("type", &s))
		{
			Link::Log("missing breakpoint type");
		}
		else if (*s == "breakpoint")
		{
			ParseBreakpoint(map, &bpList);
		}
		else
		{
			ParseOther(map, &otherList);
		}
	}
	iter.Invalidate();

	GetLink()->GetBreakpointManager()->UpdateBreakpoints(bpList, otherList);
}

/******************************************************************************
 ParseBreakpoint (private)

 *****************************************************************************/

void
gdb::GetBreakpointsCmd::ParseBreakpoint
	(
	JStringPtrMap<JString>&	map,
	JPtrArray<Breakpoint>*	list
	)
{
	JIndex bpIndex;
	Breakpoint::Action action;
	bool enabled;
	JSize ignoreCount;
	if (!ParseCommon(map, &bpIndex, &action, &enabled, &ignoreCount))
	{
		return;
	}

	JString* s;
	JString fileName;
	JIndex lineIndex = 1;
	do
	{
		if (!map.GetElement("line", &s))
		{
			Link::Log("warn: missing breakpoint line");
			break;
		}
		if (!s->ConvertToUInt(&lineIndex))
		{
			Link::Log("warn: line number is not integer");
			break;
		}

		if (!map.GetElement("file", &s))
		{
			Link::Log("warn: missing breakpoint filename");
			break;
		}
		fileName = *s;
	}
	while (false);

	JPtrArray<JString> split(JPtrArrayT::kDeleteAll);
	do
	{
		if (!fileName.IsEmpty())
		{
			break;
		}

		if (map.GetElement("original-location", &s))
		{
			if (!s->Contains(":"))
			{
				Link::Log("warn: missing line number in original-location");
				break;
			}

			s->Split(":", &split, 2);
			if (!split.GetElement(1)->IsEmpty() && !split.GetElement(2)->IsEmpty())
			{
				if (!split.GetElement(2)->ConvertToUInt(&lineIndex))
				{
					Link::Log("warn: line number is not integer in original-location");
					break;
				}

				fileName = *(split.GetElement(1));
			}
		}
	}
	while (false);

	if (fileName.IsEmpty())
	{
		Link::Log("unable to parse breakpoint location");
	}

	JString fn;
	if (map.GetElement("func", &s))
	{
		fn = *s;
	}

	JString addr;
	if (map.GetElement("addr", &s))
	{
		addr = *s;
	}

	JString condition;
	if (map.GetElement("cond", &s))
	{
		condition = *s;
	}

	auto* bp =
		jnew Breakpoint(bpIndex, fileName, lineIndex, fn, addr,
						 enabled, action, condition, ignoreCount);
	list->InsertSorted(bp);
}

/******************************************************************************
 ParseOther (private)

 *****************************************************************************/

void
gdb::GetBreakpointsCmd::ParseOther
	(
	JStringPtrMap<JString>&	map,
	JPtrArray<Breakpoint>*	list
	)
{
	JIndex bpIndex;
	Breakpoint::Action action;
	bool enabled;
	JSize ignoreCount;
	if (!ParseCommon(map, &bpIndex, &action, &enabled, &ignoreCount))
	{
		return;
	}

	JString* s;
	JString fn;
	if (map.GetElement("type", &s))
	{
		fn = *s;
	}

	JString condition;
	if (map.GetElement("what", &s))
	{
		condition = *s;
	}

	auto* bp =
		jnew Breakpoint(bpIndex, Location(), fn, JString::empty,
						 enabled, action, condition, ignoreCount);
	assert( bp != nullptr );
	list->Append(bp);

	if (fn.Contains("watchpoint"))		// may be deleted when go out of scope
	{
		(GetLink()->GetBreakpointManager())->SetUpdateWhenStop(true);
	}
}

/******************************************************************************
 ParseCommon (private)

 *****************************************************************************/

bool
gdb::GetBreakpointsCmd::ParseCommon
	(
	JStringPtrMap<JString>&	map,
	JIndex*					bpIndex,
	Breakpoint::Action*		action,
	bool*					enabled,
	JSize*					ignoreCount
	)
{
	JString* s;
	if (!map.GetElement("number", &s))
	{
		Link::Log("missing otherpoint number");
		return false;
	}
	if (!s->ConvertToUInt(bpIndex))
	{
		Link::Log("otherpoint number is not integer");
		return false;
	}

	if (!map.GetElement("disp", &s))
	{
		Link::Log("missing otherpoint action");
		return false;
	}
	if (JString::Compare(*s, "del", JString::kIgnoreCase) == 0)
	{
		*action = Breakpoint::kRemoveBreakpoint;
	}
	else if (JString::Compare(*s, "dis", JString::kIgnoreCase) == 0)
	{
		*action = Breakpoint::kDisableBreakpoint;
	}
	else	// "keep" or unknown
	{
		*action = Breakpoint::kKeepBreakpoint;
	}

	if (!map.GetElement("enabled", &s))
	{
		Link::Log("missing otherpoint enable status");
		return false;
	}
	*enabled = *s == "y" || *s == "Y";

	*ignoreCount = 0;
	if (map.GetElement("ignore", &s) && !s->IsEmpty() &&
		!s->ConvertToUInt(ignoreCount))
	{
		Link::Log("ignore count is not integer");
		return false;
	}

	// may be deleted or other status change

	if (*action != Breakpoint::kKeepBreakpoint || *ignoreCount > 0)
	{
		(GetLink()->GetBreakpointManager())->SetUpdateWhenStop(true);
	}

	return true;
}
