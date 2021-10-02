/******************************************************************************
 GDBGetThread.cpp

	BASE CLASS = GetThread

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetThread.h"
#include "ThreadsWidget.h"
#include "GDBGetThreads.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetThread::GDBGetThread
	(
	ThreadsWidget* widget
	)
	:
	GetThread(JString("set width 0\ninfo threads", JString::kNoCopy), widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetThread::~GDBGetThread()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBGetThread::HandleSuccess
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
			if (GDBGetThreads::ExtractThreadIndex(line, &currentThreadIndex))
			{
				break;
			}
		}
	}

	GetWidget()->FinishedLoading(currentThreadIndex);
}
