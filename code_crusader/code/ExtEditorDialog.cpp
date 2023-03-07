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

	auto* window = jnew JXWindow(this, 450,220, JString::empty);
	assert( window != nullptr );

	itsEditTextFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,40, 230,20);
	assert( itsEditTextFileCmdInput != nullptr );

	itsEditTextFileLineCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,60, 230,20);
	assert( itsEditTextFileLineCmdInput != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,190, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,190, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::ExtEditorDialog::shortcuts::JXLayout"));

	auto* editTextHint =
		jnew JXStaticText(JGetString("editTextHint::ExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,80, 270,20);
	assert( editTextHint != nullptr );
	editTextHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	editTextHint->SetToLabel();

	auto* editTextLabel =
		jnew JXStaticText(JGetString("editTextLabel::ExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,40, 170,20);
	assert( editTextLabel != nullptr );
	editTextLabel->SetToLabel();

	auto* editLineLabel =
		jnew JXStaticText(JGetString("editLineLabel::ExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,60, 170,20);
	assert( editLineLabel != nullptr );
	editLineLabel->SetToLabel();

	itsExtTextEditorCB =
		jnew JXTextCheckbox(JGetString("itsExtTextEditorCB::ExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 160,20);
	assert( itsExtTextEditorCB != nullptr );

	itsEditBinaryFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,130, 230,20);
	assert( itsEditBinaryFileCmdInput != nullptr );

	auto* editBinaryHint =
		jnew JXStaticText(JGetString("editBinaryHint::ExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,150, 230,20);
	assert( editBinaryHint != nullptr );
	editBinaryHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	editBinaryHint->SetToLabel();

	auto* editBinaryLabel =
		jnew JXStaticText(JGetString("editBinaryLabel::ExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,130, 170,20);
	assert( editBinaryLabel != nullptr );
	editBinaryLabel->SetToLabel();

	itsExtBinaryEditorCB =
		jnew JXTextCheckbox(JGetString("itsExtBinaryEditorCB::ExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 180,20);
	assert( itsExtBinaryEditorCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::ExtEditorDialog"));
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
