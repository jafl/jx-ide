/******************************************************************************
 RunTEScriptDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "RunTEScriptDialog.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStringHistoryMenu.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jcore/JOutPipeStream.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JSubstitute.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength = 20;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 stores itsStayOpenCB

/******************************************************************************
 Constructor

 ******************************************************************************/

RunTEScriptDialog::RunTEScriptDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kRunTEScriptDialogID)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RunTEScriptDialog::~RunTEScriptDialog()
{
	// prefs written by DeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
RunTEScriptDialog::Activate()
{
	JXWindowDirector::Activate();

	if (IsActive())
	{
		itsCmdInput->Focus();
		itsCmdInput->SelectAll();
	}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
RunTEScriptDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 390,110, JString::empty);
	assert( window != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::RunTEScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,80, 60,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::RunTEScriptDialog::shortcuts::JXLayout"));

	itsRunButton =
		jnew JXTextButton(JGetString("itsRunButton::RunTEScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 310,80, 60,20);
	assert( itsRunButton != nullptr );
	itsRunButton->SetShortcuts(JGetString("itsRunButton::RunTEScriptDialog::shortcuts::JXLayout"));

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 320,20);
	assert( itsCmdInput != nullptr );

	auto* cmdLabel =
		jnew JXStaticText(JGetString("cmdLabel::RunTEScriptDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 350,20);
	assert( cmdLabel != nullptr );
	cmdLabel->SetToLabel();

	itsHistoryMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,40, 30,20);
	assert( itsHistoryMenu != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::RunTEScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,80, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::RunTEScriptDialog::shortcuts::JXLayout"));

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::RunTEScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 90,20);
	assert( itsStayOpenCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::RunTEScriptDialog"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

	ListenTo(itsRunButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
	ListenTo(itsHistoryMenu);

	itsCmdInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsStayOpenCB->SetState(true);

	// create hidden JXDocument so Meta-# shortcuts work

	auto* fileListMenu =
		jnew JXDocumentMenu(JString::empty, window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != nullptr );

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
RunTEScriptDialog::UpdateDisplay()
{
	if (itsCmdInput->GetText()->IsEmpty())
	{
		itsRunButton->Deactivate();
	}
	else
	{
		itsRunButton->Activate();
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
RunTEScriptDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsRunButton && message.Is(JXButton::kPushed))
	{
		RunScript();
		if (!itsStayOpenCB->IsChecked())
		{
			Deactivate();
		}
	}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
	{
		Deactivate();
	}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		(JXGetHelpManager())->ShowSection("EditorHelp-ExtScript");
	}

	else if (sender == itsHistoryMenu && message.Is(JXMenu::kItemSelected))
	{
		itsHistoryMenu->UpdateInputField(message, itsCmdInput);
		itsCmdInput->Focus();
	}

	else if (sender == itsCmdInput &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
	{
		UpdateDisplay();
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 RunSimpleScript

	Runs the given script with argyments:  path file_name line_number

 ******************************************************************************/

bool
RunTEScriptDialog::RunSimpleScript
	(
	const JString&	scriptName,
	JTextEditor*	te,
	const JString&	fileName
	)
{
	JString cmd = JPrepArgForExec(scriptName);
	cmd        += " $p $n $l";
	return RunScript(cmd, te, fileName);
}

/******************************************************************************
 RunScript

 ******************************************************************************/

bool
RunTEScriptDialog::RunScript()
{
	TextDocument* textDoc = nullptr;
	if (GetDocumentManager()->GetActiveTextDocument(&textDoc))
	{
		bool onDisk;
		const JString fullName = textDoc->GetFullName(&onDisk);
		if (onDisk)
		{
			const bool result = RunScript(textDoc->GetTextEditor(), fullName);
			textDoc->Activate();
			return result;
		}
		else
		{
			JGetUserNotification()->ReportError(
				JGetString("FileDoesNotExist::RunTEScriptDialog"));
			return false;
		}
	}
	else
	{
		JGetUserNotification()->ReportError(
			JGetString("NoEditor::RunTEScriptDialog"));
		return false;
	}
}

// private

bool
RunTEScriptDialog::RunScript
	(
	JTextEditor*	te,
	const JString&	fullName
	)
{
	if (!itsCmdInput->InputValid())
	{
		return false;
	}

	itsCmdInput->GetText()->DeactivateCurrentUndo();

	if (RunScript(itsCmdInput->GetText()->GetText(), te, fullName))
	{
		itsHistoryMenu->AddString(itsCmdInput->GetText()->GetText());
		return true;
	}
	else
	{
		return false;
	}
}

bool
RunTEScriptDialog::RunScript
	(
	const JString&	origCmd,
	JTextEditor*	te,
	const JString&	fullName
	)
{
	JString cmd = origCmd;
	ReplaceVariables(&cmd, te, fullName);

	pid_t pid;
	int toFD, fromFD, errFD;
	const JError execErr =
		JExecute(cmd, &pid, kJCreatePipe, &toFD,
				 kJCreatePipe, &fromFD, kJCreatePipe, &errFD);
	if (!execErr.OK())
	{
		Activate();
		execErr.ReportIfError();
		return false;
	}
	else
	{
		JOutPipeStream output(toFD, true);
		JString text;
		if (te->GetSelection(&text))
		{
			text.Print(output);
		}
		output.close();

		// read stdout first since this is more likely to fill up

		JReadAll(fromFD, &text);

		JString msg;
		JReadAll(errFD, &msg);
		if (!msg.IsEmpty())
		{
			msg.Prepend(JGetString("Error::global"));
			JGetUserNotification()->ReportError(msg);
			return false;
		}

		text.TrimWhitespace();
		if (!text.IsEmpty())
		{
			te->Paste(text);
		}
	}

	return true;
}

/******************************************************************************
 ReplaceVariables (private)

 ******************************************************************************/

void
RunTEScriptDialog::ReplaceVariables
	(
	JString*		cmd,
	JTextEditor*	te,
	const JString&	fullName
	)
{
	JString path, fileName;
	JSplitPathAndName(fullName, &path, &fileName);

	const JString lineIndexStr(
		te->VisualLineIndexToCRLineIndex(
			te->GetLineForChar(te->GetInsertionCharIndex())),
		0);

	JSubstitute sub;
	sub.IgnoreUnrecognized();
	sub.DefineVariable("f", fullName);
	sub.DefineVariable("p", path);
	sub.DefineVariable("n", fileName);
	sub.DefineVariable("l", lineIndexStr);
	sub.Substitute(cmd);
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
RunTEScriptDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
	{
		return;
	}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsHistoryMenu->ReadSetup(input);

	if (vers >= 1)
	{
		bool stayOpen;
		input >> JBoolFromString(stayOpen);
		itsStayOpenCB->SetState(stayOpen);
	}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
RunTEScriptDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsHistoryMenu->WriteSetup(output);

	output << ' ' << JBoolToString(itsStayOpenCB->IsChecked());
	output << ' ';
}
