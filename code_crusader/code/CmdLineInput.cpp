/******************************************************************************
 CmdLineInput.cpp

	BASE CLASS = JXInputField

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CmdLineInput.h"
#include "ExecOutputDocument.h"
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CmdLineInput::CmdLineInput
	(
	ExecOutputDocument*	doc,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsDoc = doc;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CmdLineInput::~CmdLineInput()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CmdLineInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJReturnKey)
	{
		itsDoc->SendText(GetText()->GetText());
		itsDoc->SendText(JString::newline);
		GetText()->SetText(JString::empty);
	}
	else
	{
		JXInputField::HandleKeyPress(c, keySym, modifiers);
	}
}
