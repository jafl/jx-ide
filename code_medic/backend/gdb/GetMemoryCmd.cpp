/******************************************************************************
 GetMemoryCmd.cpp

	BASE CLASS = GetMemoryCmd

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "gdb/GetMemoryCmd.h"
#include "MemoryDir.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::GetMemoryCmd::GetMemoryCmd
	(
	MemoryDir* dir
	)
	:
	::GetMemoryCmd(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::GetMemoryCmd::~GetMemoryCmd()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

static const JUtf8Byte* kCommandName[] =
{
	"xb", "xh", "xw", "xg", "cb", "i"
};

void
gdb::GetMemoryCmd::Starting()
{
	::GetMemoryCmd::Starting();

	MemoryDir::DisplayType type;
	JSize count;
	const JString& expr = GetDirector()->GetExpression(&type, &count);

	JString cmd("x/");
	cmd += JString((JUInt64) count);
	cmd += kCommandName[ type-1 ];
	cmd += " ";
	cmd += expr;

	SetCommand(cmd);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex prefixPattern = "^\\s*0[xX][[:xdigit:]]+(.*):\\s";

void
gdb::GetMemoryCmd::HandleSuccess
	(
	const JString& data
	)
{
	JString s = data;

	JStringIterator iter(&s);
	while (iter.Next(prefixPattern))
	{
		const JStringMatch& m = iter.GetLastMatch();

		const JSize count = m.GetCharacterRange(1).GetCount();
		iter.SkipPrev(count + 2);
		iter.RemoveNext(count);
	}

	iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	while (iter.Next("\t"))
	{
		iter.SetPrev(JUtf8Character(' '));
	}
	iter.Invalidate();

	GetDirector()->Update(s);
}
