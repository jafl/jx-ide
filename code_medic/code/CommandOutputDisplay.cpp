/******************************************************************************
 CommandOutputDisplay.cpp

	BASE CLASS = TextDisplayBase

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "CommandOutputDisplay.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CommandOutputDisplay::CommandOutputDisplay
	(
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	TextDisplayBase(kFullEditor, false, menuBar, scrollbarSet,
					enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CommandOutputDisplay::~CommandOutputDisplay()
{
}

/******************************************************************************
 PlaceCursorAtEnd

 ******************************************************************************/

void
CommandOutputDisplay::PlaceCursorAtEnd()
{
	if (!GetText()->IsEmpty())
	{
		SetCaretLocation(GetText()->GetText().GetCharacterCount()+1);
		if (GetText()->GetText().GetLastCharacter() != '\n')
		{
			Paste(JString::newline);
		}
	}
}
