/******************************************************************************
 TEColIndexInput.cpp

	Displays the line that the caret is on.

	BASE CLASS = TECaretInputBase

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#include "TEColIndexInput.h"
#include "TELineIndexInput.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TEColIndexInput::TEColIndexInput
	(
	TELineIndexInput*	lineInput,
	JXStaticText*		label,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	TECaretInputBase(label, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsLineInput = lineInput;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TEColIndexInput::~TEColIndexInput()
{
}

/******************************************************************************
 Act (virtual protected)

 ******************************************************************************/

void
TEColIndexInput::Act
	(
	JXTEBase*		te,
	const JIndex	value
	)
{
	JInteger lineIndex;
	const bool ok = itsLineInput->GetValue(&lineIndex);
	assert( ok );

	te->GoToColumn(lineIndex, value);
}

/******************************************************************************
 GetValue (virtual protected)

 ******************************************************************************/

JIndex
TEColIndexInput::GetValue
	(
	JXTEBase* te
	)
	const
{
	return te->GetColumnForChar(te->GetInsertionCharIndex());
}

JIndex
TEColIndexInput::GetValue
	(
	const JTextEditor::CaretLocationChanged& info
	)
	const
{
	return info.GetColumnIndex();
}
