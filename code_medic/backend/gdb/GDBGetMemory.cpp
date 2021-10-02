/******************************************************************************
 GDBGetMemory.cpp

	BASE CLASS = GetMemory

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GDBGetMemory.h"
#include "MemoryDir.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetMemory::GDBGetMemory
	(
	MemoryDir* dir
	)
	:
	GetMemory(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetMemory::~GDBGetMemory()
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
GDBGetMemory::Starting()
{
	GetMemory::Starting();

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
GDBGetMemory::HandleSuccess
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

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter.Next("\t"))
	{
		iter.SetPrev(JUtf8Character(' '));
	}

	GetDirector()->Update(s);
}
