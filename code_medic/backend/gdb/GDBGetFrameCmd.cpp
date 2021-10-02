/******************************************************************************
 GDBGetFrameCmd.cpp

	BASE CLASS = GetFrameCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetFrameCmd.h"
#include "StackWidget.h"
#include "GDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetFrameCmd::GDBGetFrameCmd
	(
	StackWidget* widget
	)
	:
	GetFrameCmd(JString("-stack-info-frame", JString::kNoCopy)),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetFrameCmd::~GDBGetFrameCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex framePattern = "\\bframe=\\{";

void
GDBGetFrameCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	const JString& data = GetLastResult();

	const JStringMatch m = framePattern.Match(data, JRegex::kIgnoreSubmatches);
	if (!m.IsEmpty())
	{
		std::istringstream stream(data.GetBytes());
		stream.seekg(m.GetUtf8ByteRange().last);

		JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
		JString* s;
		JIndex frameIndex;
		if (!GDBLink::ParseMap(stream, &map))
		{
			GetLink()->Log("invalid data map");
		}
		else if (!map.GetElement("level", &s))
		{
			GetLink()->Log("missing frame index");
		}
		else if (!s->ConvertToUInt(&frameIndex))
		{
			GetLink()->Log("frame index is not integer");
		}
		else
		{
			itsWidget->SelectFrame(frameIndex);
		}
	}
	else
	{
		GetLink()->Log("GDBGetFrameCmd failed to match");
	}
}
