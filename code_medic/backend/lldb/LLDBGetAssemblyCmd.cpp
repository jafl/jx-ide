/******************************************************************************
 LLDBGetAssemblyCmd.cpp

	Get the assembly code for the current function.

	BASE CLASS = GetAssemblyCmd

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetAssemblyCmd.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "SourceDirector.h"
#include "LLDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jTextUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

lldb::GetAssemblyCmd::GetAssemblyCmd
	(
	SourceDirector* dir
	)
	:
	::GetAssemblyCmd(dir, JString::empty)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

lldb::GetAssemblyCmd::~GetAssemblyCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

static const JRegex offsetPattern = "<\\+[[:digit:]]+>$";

void
lldb::GetAssemblyCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	if (link == nullptr)
	{
		return;
	}

	SBCommandInterpreter interp = link->GetDebugger()->GetCommandInterpreter();
	if (!interp.IsValid())
	{
		return;
	}

	const Location& loc = GetDirector()->GetDisassemblyLocation();

	const JString cmd = "disassemble -n " + JPrepArgForExec(loc.GetFunctionName());
	SBCommandReturnObject result;
	interp.HandleCommand(cmd.GetBytes(), result);

	JPtrArray<JString> addrList(JPtrArrayT::kDeleteAll);
	JString instText;

	if (result.IsValid() && result.Succeeded() && result.HasResult())
	{
		std::istringstream input(result.GetOutput());
		JString line, s;
		JSize maxOffsetLength = 0;
		while (!input.eof() && !input.fail())
		{
			line = JReadLine(input);

			JStringIterator iter(&line);
			while (iter.Next(theUNIXTerminalFormatPattern))
			{
				iter.RemoveLastMatch();
			}

			iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
			iter.BeginMatch();
			if (!line.EndsWith(":") && iter.Next(":"))
			{
				s = iter.FinishMatch().GetString();
				if (s.StartsWith("->") && s.GetCharacterCount() > 2)
				{
					JStringIterator i2(&s);
					i2.RemoveNext(2);
				}
				s.TrimWhitespace();
				addrList.Append(s);

				const JStringMatch m = offsetPattern.Match(s, JRegex::kIgnoreSubmatches);
				if (!m.IsEmpty())
				{
					maxOffsetLength = JMax(maxOffsetLength, m.GetCharacterRange().GetCount());
				}

				if (!instText.IsEmpty())
				{
					instText += "\n";
				}

				iter.BeginMatch();
				iter.MoveTo(JStringIterator::kStartAtEnd, 0);
				s = iter.FinishMatch().GetString();
				s.TrimWhitespace();
				instText.Append(s);
			}
		}

		const JSize count = addrList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			JString* s = addrList.GetElement(i);

			JStringIterator iter(s);
			const JStringMatch m = offsetPattern.Match(*s, JRegex::kIgnoreSubmatches);
			if (iter.Next(offsetPattern))
			{
				const JSize count = iter.GetLastMatch().GetCharacterRange().GetCount();
				iter.SkipPrev(count-2);

				const JSize pad = maxOffsetLength - count;
				for (JUnsignedOffset j=0; j<pad; j++)
				{
					iter.Insert("0");
				}
			}
		}
	}

	GetDirector()->DisplayDisassembly(&addrList, instText);
}
