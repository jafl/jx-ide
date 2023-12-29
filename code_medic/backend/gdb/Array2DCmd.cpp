/******************************************************************************
 GDBArray2DCmd.cpp

	BASE CLASS = Array2DCmd

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "gdb/Array2DCmd.h"
#include "Array2DDir.h"
#include "VarNode.h"
#include <jx-af/jx/JXStringTable.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JStyleTableData.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

gdb::Array2DCmd::Array2DCmd
	(
	Array2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	::Array2DCmd(dir, table, data)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

gdb::Array2DCmd::~Array2DCmd()
{
}

/******************************************************************************
 PrepareToSend (virtual)

 ******************************************************************************/

void
gdb::Array2DCmd::PrepareToSend
	(
	const UpdateType	type,
	const JIndex		index,
	const JInteger		arrayIndex
	)
{
	::Array2DCmd::PrepareToSend(type, index, arrayIndex);

	JString cmd("set print pretty off\nset print array off\n"
				"set print repeats 0\nset width 0\n");

	const JIndex max =
		(GetType() == kRow ? GetData()->GetColCount() : GetData()->GetRowCount());
	assert( max > 0 );

	for (JIndex i=1; i<=max; i++)
	{
		cmd += "print ";
		cmd += GetDirector()->GetExpression(GetCell(i));
		cmd += "\n";
	}

	SetCommand(cmd);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex prefixPattern("^\\$[[:digit:]]+[[:space:]]+=[[:space:]]+(.*)$");

void
gdb::Array2DCmd::HandleSuccess
	(
	const JString& data
	)
{
	const JIndex max =
		(GetType() == kRow ? GetData()->GetColCount() : GetData()->GetRowCount());
	if (max == 0 || !ItsIndexValid())
	{
		GetDirector()->UpdateNext();
		return;
	}

	JIndex i = 1;

	JStringIterator iter(data);
	JString value;
	while (i <= max && iter.Next(prefixPattern))
	{
		value = iter.GetLastMatch().GetSubstring(1);
		value.TrimWhitespace();

		const JPoint cell        = GetCell(i);
		const JString& origValue = GetData()->GetString(cell);
		const bool isNew     = !origValue.IsEmpty() && origValue != value;

		GetData()->SetString(cell, value);
		GetTable()->SetCellStyle(cell, VarNode::GetFontStyle(true, isNew));

		i++;
	}
	iter.Invalidate();

	if (i == 1)
	{
		JString s = data;
		s.TrimWhitespace();

		JStringIterator iter2(&s);
		if (iter2.Next("\n"))
		{
			iter2.SkipPrev();
			iter2.RemoveAllNext();
			s.TrimWhitespace();
		}
		iter2.Invalidate();
		HandleFailure(1, s);
	}
	else if (i <= max)
	{
		HandleFailure(i, JString::empty);
	}

	GetDirector()->UpdateNext();
}
