/******************************************************************************
 VIKeyHandler.cpp

	Class to implement vi keybindings.

	BASE CLASS = JXVIKeyHandler

	Copyright © 2010 by John Lindal.

 ******************************************************************************/

#include "VIKeyHandler.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "TELineIndexInput.h"
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

VIKeyHandler::VIKeyHandler
	(
	TextEditor* te
	)
	:
	itsCBTE(te)
{
	Initialize(te);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

VIKeyHandler::~VIKeyHandler()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

bool
VIKeyHandler::HandleKeyPress
	(
	const JUtf8Character&			key,
	const bool					selectText,
	const JTextEditor::CaretMotion	motion,
	const bool					deleteToTabStop
	)
{
	bool result;
	if (PrehandleKeyPress(key, &result))
	{
		return result;
	}

	if (key == ':')
	{
		SetMode(kCommandLineMode);
		return true;
	}
	else if (GetMode() == kCommandLineMode && GetCommandLine().IsEmpty() &&
			 key.IsDigit() && key != '0')
	{
		TELineIndexInput* field = itsCBTE->GetLineInput();
		field->Focus();

		field->GetText()->SetText(JString(key));
		field->GoToEndOfLine();

		SetMode(kCommandMode);
		return true;
	}
	else if (GetMode() == kCommandLineMode && key == '\n')
	{
		TextDocument* doc = itsCBTE->GetDocument();
		const JString& buf  = GetCommandLine();
		if (buf == "0")
		{
			itsCBTE->SetCaretLocation(1);
		}
		else if (buf == "$")
		{
			itsCBTE->SetCaretLocation(itsCBTE->GetText()->GetText().GetCharacterCount()+1);
		}
		else if (buf == "w" || buf == "w!")
		{
			doc->SaveInCurrentFile();
		}
		else if (buf == "q")
		{
			doc->Close();
			return true;
		}
		else if (buf == "q!")
		{
			doc->DataReverted(true);
			doc->Close();
			return true;
		}
		else if (buf == "wq")
		{
			doc->SaveInCurrentFile();
			doc->Close();
			return true;
		}

		SetMode(kCommandMode);
		return true;
	}
	else if (GetMode() == kCommandLineMode)
	{
		AppendToCommandLine(key);
		return true;
	}

	else
	{
		return JXVIKeyHandler::HandleKeyPress(key, selectText, motion, deleteToTabStop);
	}
}
