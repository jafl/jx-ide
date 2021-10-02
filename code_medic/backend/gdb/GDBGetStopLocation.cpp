/******************************************************************************
 GDBGetStopLocation.cpp

	BASE CLASS = Command

	Copyright (C) 2009-11 by John Lindal.

 ******************************************************************************/

#include "GDBGetStopLocation.h"
#include "GDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetStopLocation::GDBGetStopLocation()
	:
	Command("-stack-info-frame", false, false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetStopLocation::~GDBGetStopLocation()
{
}

/******************************************************************************
 GetLocation (protected)

 ******************************************************************************/

static const JRegex locationPattern = "\\bframe=\\{";

Location
GDBGetStopLocation::GetLocation()
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
		const bool parsed = GDBLink::ParseMap(stream, &map);
		if (!parsed)
		{
			GetLink()->Log("invalid data map");
		}
		else if (!map.GetElement("fullname", &fullName))
		{
			GetLink()->Log("missing file name");
		}
		else if (!map.GetElement("line", &s))
		{
			GetLink()->Log("missing line index");
		}
		else if (!s->ConvertToUInt(&lineIndex))
		{
			GetLink()->Log("line index is not integer");
		}
		else
		{
			loc.SetFileName(*fullName);
			loc.SetLineNumber(lineIndex);
		}

		if (parsed && map.GetElement("func", &s) && map.GetElement("addr", &s1))
		{
			loc.SetFunctionName(*s);
			loc.SetMemoryAddress(*s1);
		}
	}
	else
	{
		GetLink()->Log("GDBGetStopLocation failed to match");
	}

	return loc;
}
