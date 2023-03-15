/******************************************************************************
 ShellEditor.cpp

	BASE CLASS = TextEditor

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#include "ShellEditor.h"
#include "ShellDocument.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jTextUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ShellEditor::ShellEditor
	(
	TextDocument*		document,
	const JString&		fileName,
	JXMenuBar*			menuBar,
	TELineIndexInput*	lineInput,
	TEColIndexInput*	colInput,
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
	TextEditor(document, fileName, menuBar, lineInput, colInput, true,
				 scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	itsInsertIndex(1,1)
{
	itsShellDoc = (ShellDocument*) document;

	GetText()->SetDefaultFontStyle(JFontStyle(true, false, 0, false));

	itsInsertFont = GetText()->GetDefaultFont();
	itsInsertFont.SetBold(false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ShellEditor::~ShellEditor()
{
}

/******************************************************************************
 InsertText

 ******************************************************************************/

void
ShellEditor::InsertText
	(
	const JString& text
	)
{
	CaretLocation savedCaret;
	const bool hadCaret = GetCaretLocation(&savedCaret);

	SetCaretLocation(itsInsertIndex);
	SetCurrentFont(itsInsertFont);
	const JStyledText::TextRange range =
		JPasteUNIXTerminalOutput(text, itsInsertIndex, GetText());
	SetCurrentFont(GetText()->GetDefaultFont());
	GetText()->ClearUndo();

	itsInsertIndex = range.GetAfter();
	if (hadCaret)
	{
		savedCaret.location.charIndex += range.charRange.GetCount();
		savedCaret.location.byteIndex += range.byteRange.GetCount();
		SetCaretLocation(savedCaret.location);
	}

	const JStyledText::TextIndex i = GetText()->AdjustTextIndex(itsInsertIndex, -1);
	if (GetText()->GetFont(i.charIndex).GetStyle().bold)
	{
		GetText()->SetFontBold(JStyledText::TextRange(i, itsInsertIndex), false, true);
	}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
ShellEditor::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	const bool controlOn = modifiers.control();
	const bool metaOn    = modifiers.meta();
	const bool shiftOn   = modifiers.shift();
	if ((c == kJLeftArrow && metaOn && !controlOn && !shiftOn) ||
		(c == JXCtrl('A') && controlOn && !metaOn && !shiftOn))
	{
		JRunArrayIterator iter(
			GetText()->GetStyles(),
			kJIteratorStartBefore, GetInsertionIndex().charIndex);

		JFont f;
		if (iter.Prev(&f) && f == GetText()->GetDefaultFont())
		{
			SetCaretLocation(iter.GetRunStart());
			return;
		}
	}

	if (c == kJReturnKey)
	{
		SetCurrentFont(itsInsertFont);
	}
	else
	{
		SetCurrentFont(GetText()->GetDefaultFont());
	}

	bool sentCmd = false;
	if (c == kJReturnKey && !modifiers.shift() && !HasSelection())
	{
		JIndex index;
		bool ok = GetCaretLocation(&index);
		assert( ok );

		JString cmd;

		JRunArrayIterator fiter(GetText()->GetStyles(), kJIteratorStartBefore, index);
		JFont f;
		if (fiter.Prev(&f) && f == GetText()->GetDefaultFont())
		{
			const JIndex endIndex = fiter.GetRunEnd();

			JStringIterator siter(GetText()->GetText(), kJIteratorStartBefore, fiter.GetRunStart());
			siter.BeginMatch();
			siter.MoveTo(kJIteratorStartBefore, endIndex);
			cmd = siter.FinishMatch().GetString();
			SetCaretLocation(endIndex);
			siter.Invalidate();

			if (cmd.StartsWith("man "))
			{
				JStringIterator iter(&cmd);
				iter.RemoveNext(3);
				cmd.Prepend("jcc --man");
			}
			else if (cmd.StartsWith("apropos "))
			{
				JStringIterator iter(&cmd);
				iter.RemoveNext(7);
				cmd.Prepend("jcc --apropos");
			}
			else if (cmd.StartsWith("vi "))
			{
				JStringIterator iter(&cmd);
				iter.RemoveNext(2);
				cmd.Prepend("jcc");
			}
			else if (cmd.StartsWith("less ") || cmd.StartsWith("more "))
			{
				JStringIterator iter(&cmd);
				iter.RemoveNext(4);
				cmd.Prepend("jcc");
			}
			else if (cmd == "more" || cmd == "less" || cmd == "vi")
			{
				cmd = "jcc";
			}
		}

		cmd += "\n";
		itsShellDoc->SendCommand(cmd);

		sentCmd = true;
	}

	TextEditor::HandleKeyPress(c, keySym, modifiers);

	if (sentCmd)
	{
		itsInsertIndex = GetInsertionIndex();
	}
}
