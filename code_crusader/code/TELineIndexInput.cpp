/******************************************************************************
 TELineIndexInput.cpp

	Displays the line that the caret is on.

	BASE CLASS = TECaretInputBase

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#include "TELineIndexInput.h"
#include <jx-af/jx/jXMenuUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TELineIndexInput::TELineIndexInput
	(
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
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TELineIndexInput::~TELineIndexInput()
{
}

/******************************************************************************
 SetTE

	We are created before the column input so we can't get the pointer
	in our constructor.

 ******************************************************************************/

void
TELineIndexInput::SetTE
	(
	JXTEBase*			te,
	TECaretInputBase*	colInput
	)
{
	itsColInput = colInput;
	TECaretInputBase::SetTE(te);
}

/******************************************************************************
 HandleKeyPress (virtual)

	Shift-Return switches focus to the column input.

	Meta-Return converts from hard to soft line breaks.

 ******************************************************************************/

void
TELineIndexInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if ((c == '\r' || c == '\n') &&
		!modifiers.meta() && !modifiers.control() && modifiers.shift())
	{
		ShouldAct(true);
		itsColInput->Focus();	// trigger HandleUnfocusEvent()
		ShouldAct(false);
	}
	else if ((c == '\r' || c == '\n') &&
			 !modifiers.GetState(JXAdjustNMShortcutModifier(kJXControlKeyIndex)) &&
			  modifiers.GetState(JXAdjustNMShortcutModifier(kJXMetaKeyIndex))    &&
			 !modifiers.shift())
	{
		JXTEBase* te = GetTE();
		JInteger value;
		if (JXIntegerInput::GetValue(&value))
		{
			const JString s((JUInt64) te->CRLineIndexToVisualLineIndex(value));
			GetText()->SetText(s);
		}

		ShouldAct(true);
		te->Focus();				// trigger HandleUnfocusEvent()
		ShouldAct(false);
	}
	else
	{
		TECaretInputBase::HandleKeyPress(c, keySym, modifiers);

		// vi emulation

		const JString& s = GetText()->GetText();
		if (s == "0")
		{
			GetText()->SetText("1");
			GoToEndOfLine();
		}
		if (s == "$")
		{
			GetText()->SetText(JString((JUInt64) GetTE()->GetLineCount()+1));
			GoToEndOfLine();
		}
	}
}

/******************************************************************************
 Act (virtual protected)

 ******************************************************************************/

void
TELineIndexInput::Act
	(
	JXTEBase*		te,
	const JIndex	value
	)
{
	te->GoToLine(value);
}

/******************************************************************************
 GetValue (virtual protected)

 ******************************************************************************/

JIndex
TELineIndexInput::GetValue
	(
	JXTEBase* te
	)
	const
{
	return te->GetLineForChar(te->GetInsertionCharIndex());
}

JIndex
TELineIndexInput::GetValue
	(
	const JTextEditor::CaretLocationChanged& info
	)
	const
{
	return info.GetLineIndex();
}
