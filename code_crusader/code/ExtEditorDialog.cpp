/******************************************************************************
 ExtEditorDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "ExtEditorDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXCSFDialogBase.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ExtEditorDialog::ExtEditorDialog
	(
	const bool		editTextLocally,
	const JString&	editTextFileCmd,
	const JString&	editTextFileLineCmd,
	const bool		editBinaryLocally,
	const JString&	editBinaryFileCmd
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(editTextLocally, editTextFileCmd, editTextFileLineCmd,
				editBinaryLocally, editBinaryFileCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ExtEditorDialog::~ExtEditorDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
ExtEditorDialog::BuildWindow
	(
	const bool	editTextLocally,
	const JString&	editTextFileCmd,
	const JString&	editTextFileLineCmd,
	const bool	editBinaryLocally,
	const JString&	editBinaryFileCmd
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,180, JGetString("WindowTitle::ExtEditorDialog::JXLayout"));

	itsExtTextEditorCB =
		jnew JXTextCheckbox(JGetString("itsExtTextEditorCB::ExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 160,20);

	auto* editTextLabel =
		jnew JXStaticText(JGetString("editTextLabel::ExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,40, 170,20);
	editTextLabel->SetToLabel(false);

	auto* editLineLabel =
		jnew JXStaticText(JGetString("editLineLabel::ExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,60, 170,20);
	editLineLabel->SetToLabel(false);

	itsExtBinaryEditorCB =
		jnew JXTextCheckbox(JGetString("itsExtBinaryEditorCB::ExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,90, 180,20);

	auto* editBinaryLabel =
		jnew JXStaticText(JGetString("editBinaryLabel::ExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,110, 170,20);
	editBinaryLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,150, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 269,149, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::ExtEditorDialog::JXLayout"));

	itsEditTextFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 200,40, 230,20);
	itsEditTextFileCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsEditTextFileCmdInput->SetHint(JGetString("itsEditTextFileCmdInput::ExtEditorDialog::JXLayout"));

	itsEditTextFileLineCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 200,60, 230,20);
	itsEditTextFileLineCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsEditTextFileLineCmdInput->SetHint(JGetString("itsEditTextFileLineCmdInput::ExtEditorDialog::JXLayout"));

	itsEditBinaryFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 200,110, 230,20);
	itsEditBinaryFileCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsEditBinaryFileCmdInput->SetHint(JGetString("itsEditBinaryFileCmdInput::ExtEditorDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsExtTextEditorCB->SetState(!editTextLocally);
	itsEditTextFileCmdInput->GetText()->SetText(editTextFileCmd);
	itsEditTextFileCmdInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	itsEditTextFileLineCmdInput->GetText()->SetText(editTextFileLineCmd);
	itsEditTextFileLineCmdInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);

	itsExtBinaryEditorCB->SetState(!editBinaryLocally);
	itsEditBinaryFileCmdInput->GetText()->SetText(editBinaryFileCmd);
	itsEditBinaryFileCmdInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);

	// until we have a built-in one
	itsExtBinaryEditorCB->SetState(true);
	itsExtBinaryEditorCB->Deactivate();

	UpdateDisplay();
	ListenTo(itsExtTextEditorCB);
	ListenTo(itsExtBinaryEditorCB);
}

/******************************************************************************
 GetPrefs

 ******************************************************************************/

void
ExtEditorDialog::GetPrefs
	(
	bool*	editTextLocally,
	JString*	editTextFileCmd,
	JString*	editTextFileLineCmd,
	bool*	editBinaryLocally,
	JString*	editBinaryFileCmd
	)
	const
{
	*editTextLocally     = !itsExtTextEditorCB->IsChecked();
	*editTextFileCmd     = itsEditTextFileCmdInput->GetText()->GetText();
	*editTextFileLineCmd = itsEditTextFileLineCmdInput->GetText()->GetText();

	*editBinaryLocally   = !itsExtBinaryEditorCB->IsChecked();
	*editBinaryFileCmd   = itsEditBinaryFileCmdInput->GetText()->GetText();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
ExtEditorDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if ((sender == itsExtTextEditorCB || sender == itsExtBinaryEditorCB) &&
		message.Is(JXCheckbox::kPushed))
	{
		UpdateDisplay();
	}
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
ExtEditorDialog::UpdateDisplay()
{
	if (itsExtTextEditorCB->IsChecked())
	{
		itsEditTextFileCmdInput->Activate();
		itsEditTextFileCmdInput->SetIsRequired(true);

		itsEditTextFileLineCmdInput->Activate();
		itsEditTextFileLineCmdInput->SetIsRequired(true);
	}
	else
	{
		itsEditTextFileCmdInput->Deactivate();
		itsEditTextFileCmdInput->SetIsRequired(false);

		itsEditTextFileLineCmdInput->Deactivate();
		itsEditTextFileLineCmdInput->SetIsRequired(false);
	}

	if (itsExtBinaryEditorCB->IsChecked())
	{
		itsEditBinaryFileCmdInput->Activate();
		itsEditBinaryFileCmdInput->SetIsRequired(true);
	}
	else
	{
		itsEditBinaryFileCmdInput->Deactivate();
		itsEditBinaryFileCmdInput->SetIsRequired(false);
	}
}
