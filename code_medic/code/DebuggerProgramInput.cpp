/******************************************************************************
 DebuggerProgramInput.cpp

	Input field for entering the debugger program to use.  Any executable
	on PATH is allowed, in addition to a full path.

	BASE CLASS = JXFileInput

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "DebuggerProgramInput.h"
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DebuggerProgramInput::DebuggerProgramInput
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
	JXFileInput(jnew StyledText(this, enclosure->GetFontManager()),
				enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DebuggerProgramInput::~DebuggerProgramInput()
{
}

/******************************************************************************
 GetFile (virtual)

	Returns true if the current file name is valid.  In this case, *fullName
	is set to the full path + name, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

bool
DebuggerProgramInput::GetFile
	(
	JString* fullName
	)
	const
{
	const JString& text = GetText().GetText();
	if (JProgramAvailable(text))
	{
		*fullName = text;
		return true;
	}
	else
	{
		fullName->Clear();
		return false;
	}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

bool
DebuggerProgramInput::InputValid()
{
	const JString& text = GetText()->GetText();
	return JProgramAvailable(text);
}

/******************************************************************************
 ComputeErrorLength (virtual protected)

	Return the number of characters that should be marked as invalid.

 ******************************************************************************/

JSize
DebuggerProgramInput::StyledText::ComputeErrorLength
	(
	JXFSInputBase*	field,
	const JSize		totalLength,	// original character count
	const JString&	fullName		// modified path
	)
	const
{
	const JString& text = field->GetText()->GetText();
	return JProgramAvailable(text) ? 0 : totalLength;
}
