/******************************************************************************
 FindFileDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "FindFileDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStringHistoryMenu.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXCSFDialogBase.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength = 20;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 stores itsStayOpenCB

/******************************************************************************
 Constructor

 ******************************************************************************/

FindFileDialog::FindFileDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kFindFilePrefID)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FindFileDialog::~FindFileDialog()
{
	// prefs written by DeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
FindFileDialog::Activate()
{
	JXWindowDirector::Activate();

	if (IsActive())
	{
		itsFileName->Focus();
		itsFileName->SelectAll();
	}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
FindFileDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 290,140, JGetString("WindowTitle::FindFileDialog::JXLayout"));

	auto* nameLabel =
		jnew JXStaticText(JGetString("nameLabel::FindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 80,20);
	nameLabel->SetToLabel(false);

	itsFileHistoryMenu =
		jnew JXStringHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 240,40, 30,20);

	itsIgnoreCaseCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreCaseCB::FindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 230,20);
	itsIgnoreCaseCB->SetShortcuts(JGetString("itsIgnoreCaseCB::shortcuts::FindFileDialog::JXLayout"));

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::FindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,110, 90,20);

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::FindFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,110, 60,20);
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::shortcuts::FindFileDialog::JXLayout"));

	itsFindButton =
		jnew JXTextButton(JGetString("itsFindButton::FindFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 209,109, 62,22);
	itsFindButton->SetShortcuts(JGetString("itsFindButton::shortcuts::FindFileDialog::JXLayout"));

	itsFileName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 220,20);
	itsFileName->SetFont(JFontManager::GetDefaultMonospaceFont());

// end JXLayout

	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

	ListenTo(itsFindButton);
	ListenTo(itsCloseButton);
	ListenTo(itsFileHistoryMenu);

	itsFileName->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsFileName);

	itsIgnoreCaseCB->SetState(true);
	itsStayOpenCB->SetState(true);

	// create hidden JXDocument so Meta-# shortcuts work

	jnew JXDocumentMenu(JString::empty, window,
					   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
FindFileDialog::UpdateDisplay()
{
	if (itsFileName->GetText()->IsEmpty())
	{
		itsFindButton->Deactivate();
	}
	else
	{
		itsFindButton->Activate();
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
FindFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFindButton && message.Is(JXButton::kPushed))
	{
		if (FindFile() && !itsStayOpenCB->IsChecked())
		{
			Deactivate();
		}
	}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
	{
		Deactivate();
	}

	else if (sender == itsFileHistoryMenu && message.Is(JXMenu::kItemSelected))
	{
		itsFileHistoryMenu->UpdateInputField(message, itsFileName);
		itsFileName->Focus();
	}

	else if (sender == itsFileName &&
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
 FindFile (private)

 ******************************************************************************/

bool
FindFileDialog::FindFile()
{
	JString fileName = itsFileName->GetText()->GetText();
	fileName.TrimWhitespace();
	itsFileName->GetText()->SetText(fileName);

	JIndexRange lineRange;
	JExtractFileAndLine(itsFileName->GetText()->GetText(), &fileName,
						&(lineRange.first), &(lineRange.last));
	if (itsFileName->InputValid() &&
		GetApplication()->FindAndViewFile(
			fileName, lineRange,
			itsIgnoreCaseCB->IsChecked() ? JString::kIgnoreCase : JString::kCompareCase))
	{
		itsFileHistoryMenu->AddString(fileName);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
FindFileDialog::ReadPrefs
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

	itsFileHistoryMenu->ReadSetup(input);

	bool ignoreCase;
	input >> JBoolFromString(ignoreCase);
	itsIgnoreCaseCB->SetState(ignoreCase);

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
FindFileDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsFileHistoryMenu->WriteSetup(output);

	output << ' ' << JBoolToString(itsIgnoreCaseCB->IsChecked())
				  << JBoolToString(itsStayOpenCB->IsChecked());
	output << ' ';
}
