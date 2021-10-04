/******************************************************************************
 GDBGetThreadCmd.cpp

	BASE CLASS = GetThreadCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetThreadCmd.h"
#include "ThreadsWidget.h"
#include "GDBGetThreadsCmd.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetThreadCmd::GetThreadCmd
	(
	ThreadsWidget* widget
	)
	:
	::GetThreadCmd(JString("set width 0\ninfo threads", JString::kNoCopy), widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetThreadCmd::~GetThreadCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
gdb::GetThreadCmd::HandleSuccess
	(
	const JString& data
	)
{
	JIndex currentThreadIndex = 0;

	const std::string s(data.GetRawBytes(), data.GetByteCount());
	std::istringstream input(s);
	JString line;
	while (true)
	{
		line = JReadLine(input);
		if (input.eof() || input.fail())
		{
			break;
		}

		line.TrimWhitespace();
		if (!line.IsEmpty() && line.GetFirstCharacter() == '*')
		{
			JStringIterator iter(&line);
			iter.RemoveNext();
			line.TrimWhitespace();
			if (GetThreadsCmd::ExtractThreadIndex(line, &currentThreadIndex))
			{
				break;
			}
		}
	}

	GetWidget()->FinishedLoading(currentThreadIndex);
}
