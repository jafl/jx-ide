/******************************************************************************
 GDBGetThreadsCmd.cpp

	BASE CLASS = GetThreadsCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetThreadsCmd.h"
#include "ThreadsWidget.h"
#include "ThreadNode.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetThreadsCmd::GetThreadsCmd
	(
	JTree*			tree,
	ThreadsWidget*	widget
	)
	:
	::GetThreadsCmd(JString("set width 0\ninfo threads", JString::kNoCopy), widget),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetThreadsCmd::~GetThreadsCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex prefixPattern      = "^([[:digit:]]+)[[:space:]]+";
static const JRegex threadIndexPattern = "^[0-9]{2}:";

void
gdb::GetThreadsCmd::HandleSuccess
	(
	const JString& data
	)
{
	JPtrArray<JString> threadList(JPtrArrayT::kDeleteAll);
	threadList.SetCompareFunction(CompareThreadIndices);
	threadList.SetSortOrder(JListT::kSortAscending);

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
		if (!line.IsEmpty())
		{
			if (line.GetFirstCharacter() == '*')
			{
				JStringIterator iter1(&line);
				iter1.RemoveNext();
				line.TrimWhitespace();
				ExtractThreadIndex(line, &currentThreadIndex);
			}

			JStringIterator iter2(&line);
			if (iter2.Next(prefixPattern))
			{
				iter2.ReplaceLastMatch(iter2.GetLastMatch().GetSubstring(1) + ":  ");
				iter2.Invalidate();

				while (!threadIndexPattern.Match(line))
				{
					line.Prepend("0");
				}

				auto* s = jnew JString(line);
				threadList.InsertSorted(s);
			}
		}
	}

	JTreeNode* root   = itsTree->GetRoot();
	const JSize count = threadList.GetElementCount();
	JIndex threadIndex, lineIndex;
	JString fileName;
	for (JIndex i=1; i<=count; i++)
	{
		const JString* line = threadList.GetElement(i);

		if (ExtractThreadIndex(*line, &threadIndex))
		{
			ExtractLocation(*line, &fileName, &lineIndex);

			auto* node = jnew ThreadNode(threadIndex, *line, fileName, lineIndex);

			root->Append(node);
		}
	}

	GetWidget()->FinishedLoading(currentThreadIndex);
}

/******************************************************************************
 ExtractThreadIndex (static private)

 ******************************************************************************/

static const JRegex indexPattern = "^[[:digit:]]+";

bool
gdb::GetThreadsCmd::ExtractThreadIndex
	(
	const JString&	line,
	JIndex*			threadIndex
	)
{
	const JStringMatch m = indexPattern.Match(line, JRegex::kIgnoreSubmatches);
	if (!m.IsEmpty())
	{
		const bool ok = m.GetString().ConvertToUInt(threadIndex);
		assert( ok );

		return true;
	}
	else
	{
		*threadIndex = 0;
		return false;
	}
}

/******************************************************************************
 ExtractLocation (static private)

 ******************************************************************************/

static const JRegex locationPattern = "[ \t]+at[ \t]+(.+):([[:digit:]]+)$";

bool
gdb::GetThreadsCmd::ExtractLocation
	(
	const JString&	line,
	JString*		fileName,
	JIndex*			lineIndex
	)
{
	const JStringMatch m = locationPattern.Match(line, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
	{
		*fileName = m.GetSubstring(1);

		const bool ok = m.GetSubstring(2).ConvertToUInt(lineIndex);
		assert( ok );

		return true;
	}
	else
	{
		fileName->Clear();
		*lineIndex = 0;
		return false;
	}
}

/******************************************************************************
 CompareThreadIndices (static private)

 ******************************************************************************/

std::weak_ordering
gdb::GetThreadsCmd::CompareThreadIndices
	(
	JString* const & l1,
	JString* const & l2
	)
{
	JIndex i1, i2;
	if (!ExtractThreadIndex(*l1, &i1) ||
		!ExtractThreadIndex(*l2, &i1))
	{
		return std::weak_ordering::less;
	}
	else
	{
		return JCompareIndices(i1, i2);
	}
}
