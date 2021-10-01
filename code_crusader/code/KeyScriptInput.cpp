/******************************************************************************
 KeyScriptInput.cpp

	Input field for entering a keystroke script.

	BASE CLASS = JXInputField

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "KeyScriptInput.h"
#include "MacroManager.h"
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

KeyScriptInput::KeyScriptInput
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
	JXInputField(jnew StyledText(enclosure->GetFontManager()),
				 enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

KeyScriptInput::~KeyScriptInput()
{
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Draw the illegal backslash characters in red.

 ******************************************************************************/

void
KeyScriptInput::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const bool			deletion
	)
{
	MacroManager::HighlightErrors(text, styles);
	*redrawRange = JStyledText::TextRange(
		JCharacterRange(1, text.GetCharacterCount()),
		JUtf8ByteRange(1, text.GetByteCount()));
}
