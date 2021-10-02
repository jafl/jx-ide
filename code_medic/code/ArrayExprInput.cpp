/******************************************************************************
 ArrayExprInput.cpp

	BASE CLASS = JXInputField

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "ArrayExprInput.h"
#include "TextDisplayBase.h"
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* ArrayExprInput::kReturnKeyPressed =
	"ReturnKeyPressed::ArrayExprInput";

/******************************************************************************
 Constructor

 ******************************************************************************/

ArrayExprInput::ArrayExprInput
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
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	TextDisplayBase::AdjustFont(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ArrayExprInput::~ArrayExprInput()
{
}

/******************************************************************************
 HandleKeyPress (public)

 ******************************************************************************/

void
ArrayExprInput::HandleKeyPress
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
		JXInputField::HandleKeyPress(c, keySym, modifiers);
	}
}
