/******************************************************************************
 ArrayIndexInput.cpp

	BASE CLASS = JXIntegerInput

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "ArrayIndexInput.h"
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* ArrayIndexInput::kReturnKeyPressed =
	"ReturnKeyPressed::ArrayIndexInput";

/******************************************************************************
 Constructor

 ******************************************************************************/

ArrayIndexInput::ArrayIndexInput
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXIntegerInput(enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ArrayIndexInput::~ArrayIndexInput()
{
}

/******************************************************************************
 HandleKeyPress (public)

 ******************************************************************************/

void
ArrayIndexInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJReturnKey)
	{
		Broadcast(ReturnKeyPressed());
	}
	else
	{
		JXIntegerInput::HandleKeyPress(c, keySym, modifiers);
	}
}
