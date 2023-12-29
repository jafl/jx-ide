/******************************************************************************
 GetStopLocationCmd.cpp

	BASE CLASS = Command

	Copyright (C) 2009-11 by John Lindal.

 ******************************************************************************/

#include "gdb/GetStopLocationCmd.h"
#include "gdb/Link.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetStopLocationCmd::GetStopLocationCmd()
	:
	Command("-stack-info-frame", false, false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetStopLocationCmd::~GetStopLocationCmd()
{
}

/******************************************************************************
 GetLocation (protected)

 ******************************************************************************/

static const JRegex locationPattern("\\bframe=\\{");

Location
gdb::GetStopLocationCmd::GetLocation()
	const
{
	const JString& data = GetLastResult();

	Location loc;
	const JStringMatch m = locationPattern.Match(data, JRegex::kIgnoreSubmatches);
	if (!m.IsEmpty())
	{
		std::istringstream stream(data.GetBytes());
		stream.seekg(m.GetUtf8ByteRange().last);

		JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
		JString *s, *s1, *fullName;
		JIndex lineIndex;
		const bool parsed = Link::ParseMap(stream, &map);
		if (!parsed)
		{
			Link::Log("invalid data map");
		}
		else if (!map.GetItem("fullname", &fullName))
		{
			Link::Log("missing file name");
		}
		else if (!map.GetItem("line", &s))
		{
			Link::Log("missing line index");
		}
		else if (!s->ConvertToUInt(&lineIndex))
		{
			Link::Log("line index is not integer");
		}
		else
		{
			loc.SetFileName(*fullName);
			loc.SetLineNumber(lineIndex);
		}

		if (parsed && map.GetItem("func", &s) && map.GetItem("addr", &s1))
		{
			loc.SetFunctionName(*s);
			loc.SetMemoryAddress(*s1);
		}
	}
	else
	{
		Link::Log("GDBGetStopLocationCmd failed to match");
	}

	return loc;
}
