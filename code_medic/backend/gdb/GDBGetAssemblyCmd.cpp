/******************************************************************************
 GDBGetAssemblyCmd.cpp

	Get the assembly code for the current function.

	BASE CLASS = GetAssemblyCmd

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GDBGetAssemblyCmd.h"
#include "SourceDirector.h"
#include "GDBLink.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetAssemblyCmd::GetAssemblyCmd
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

gdb::GetAssemblyCmd::~GetAssemblyCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
gdb::GetAssemblyCmd::Starting()
{
	::GetAssemblyCmd::Starting();

	const Location& loc = GetDirector()->GetDisassemblyLocation();

	JString cmd("-data-disassemble");
	if (loc.GetFileName().IsEmpty())
	{
		cmd += " -s ";
		cmd += loc.GetFunctionName();
	}
	else
	{
		JString path, name;
		JSplitPathAndName(loc.GetFileName(), &path, &name);

		cmd += " -f ";
		cmd += name;
		cmd += " -l ";
		cmd += JString((JUInt64) loc.GetLineNumber());
	}

	cmd += " -- 0";
	SetCommand(cmd);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

static const JRegex bpPattern     = "asm_insns=\\[";
static const JRegex offsetPattern = "<\\+[[:digit:]]+>$";

void
gdb::GetAssemblyCmd::HandleSuccess
	(
	const JString& cmdData
	)
{
	JPtrArray<JString> addrList(JPtrArrayT::kDeleteAll);
	JString instText;

	const JString& data = GetLastResult();
	std::istringstream stream(data.GetBytes());

	JPtrArray< JStringPtrMap<JString> > list(JPtrArrayT::kDeleteAll);
	JSize maxOffsetLength = 0;

	const JStringMatch m = bpPattern.Match(data, JRegex::kIgnoreSubmatches);
	if (!m.IsEmpty())
	{
		stream.seekg(m.GetUtf8ByteRange().last);
		if (!Link::ParseMapArray(stream, &list))
		{
			GetLink()->Log("invalid data map");
		}
		else
		{
			const JSize count = list.GetElementCount();
			JString *addr, *offset, *inst;
			for (JIndex i=1; i<=count; i++)
			{
				JStringPtrMap<JString>* map = list.GetElement(i);

				if (!map->GetElement("address", &addr))
				{
					GetLink()->Log("invalid assembly instruction: missing address");
					continue;
				}

				if (!map->GetElement("offset", &offset))
				{
					GetLink()->Log("invalid assembly instruction: missing address");
					continue;
				}

				if (!map->GetElement("inst", &inst))
				{
					GetLink()->Log("invalid assembly instruction: missing inst");
					continue;
				}

				*addr += " <+";
				*addr += *offset;
				*addr += ">";
				addrList.Append(*addr);

				maxOffsetLength = JMax(maxOffsetLength, offset->GetCharacterCount() + 3);

				if (!instText.IsEmpty())
				{
					instText.Append("\n");
				}
				instText.Append(*inst);
			}
		}
	}

	const JSize count = addrList.GetElementCount();
	for (JIndex i=1; i<count; i++)
	{
		JString* s = addrList.GetElement(i);

		const JStringMatch m2 = offsetPattern.Match(*s, JRegex::kIgnoreSubmatches);
		if (!m2.IsEmpty())
		{
			const JSize pad = maxOffsetLength - m2.GetCharacterRange().GetCount();

			JStringIterator iter(s, kJIteratorStartBefore, m2.GetCharacterRange().first + 2);
			for (JUnsignedOffset j=0; j<pad; j++)
			{
				iter.Insert("0");
			}
		}
	}

	GetDirector()->DisplayDisassembly(&addrList, instText);
}
