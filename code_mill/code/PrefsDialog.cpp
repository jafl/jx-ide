/******************************************************************************
 PrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2002 by Glenn W. Bach.

 ******************************************************************************/

#include "PrefsDialog.h"
#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PrefsDialog::PrefsDialog
	(
	JXDirector*	supervisor,
	const JString& header,
	const JString& source,
	const JString& constructor,
	const JString& destructor,
	const JString& function
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow(header, source, constructor, destructor, function);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PrefsDialog::~PrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
PrefsDialog::BuildWindow
	(
	const JString& header,
	const JString& source,
	const JString& constructor,
	const JString& destructor,
	const JString& function
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 510,530, JString::empty);
	assert( window != nullptr );

	auto* headerCommentLabel =
		jnew JXStaticText(JGetString("headerCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 470,20);
	assert( headerCommentLabel != nullptr );
	headerCommentLabel->SetToLabel();

	itsHeaderInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 470,60);
	assert( itsHeaderInput != nullptr );

	auto* sourceCommentLabel =
		jnew JXStaticText(JGetString("sourceCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,115, 470,20);
	assert( sourceCommentLabel != nullptr );
	sourceCommentLabel->SetToLabel();

	itsSourceInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,135, 470,60);
	assert( itsSourceInput != nullptr );

	auto* ctorCommentLabel =
		jnew JXStaticText(JGetString("ctorCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,210, 470,20);
	assert( ctorCommentLabel != nullptr );
	ctorCommentLabel->SetToLabel();

	itsConstructorInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,230, 470,60);
	assert( itsConstructorInput != nullptr );

	auto* dtorCommentLabel =
		jnew JXStaticText(JGetString("dtorCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,305, 470,20);
	assert( dtorCommentLabel != nullptr );
	dtorCommentLabel->SetToLabel();

	itsDestructorInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,325, 470,60);
	assert( itsDestructorInput != nullptr );

	auto* fnCommentLabel =
		jnew JXStaticText(JGetString("fnCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,400, 470,20);
	assert( fnCommentLabel != nullptr );
	fnCommentLabel->SetToLabel();

	itsFunctionInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,420, 470,60);
	assert( itsFunctionInput != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 110,500, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::PrefsDialog::shortcuts::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 330,500, 70,20);
	assert( okButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 220,500, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::PrefsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::PrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsHeaderInput->SetIsRequired();
	itsSourceInput->SetIsRequired();
	itsConstructorInput->SetIsRequired();
	itsDestructorInput->SetIsRequired();
	itsFunctionInput->SetIsRequired();

	itsHeaderInput->GetText()->SetText(header);
	itsSourceInput->GetText()->SetText(source);
	itsConstructorInput->GetText()->SetText(constructor);
	itsDestructorInput->GetText()->SetText(destructor);
	itsFunctionInput->GetText()->SetText(function);

}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
PrefsDialog::GetValues
	(
	JString* header,
	JString* source,
	JString* constructor,
	JString* destructor,
	JString* function
	)
{
	*header			= itsHeaderInput->GetText()->GetText();
	*source			= itsSourceInput->GetText()->GetText();
	*constructor	= itsConstructorInput->GetText()->GetText();
	*destructor		= itsDestructorInput->GetText()->GetText();
	*function		= itsFunctionInput->GetText()->GetText();
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 *****************************************************************************/

bool
PrefsDialog::OKToDeactivate()
{
	if (Cancelled())
	{
		return JXDialogDirector::OKToDeactivate();
	}

	return (JXDialogDirector::OKToDeactivate() &&
			(!itsHeaderInput->InputValid()      ||
			 !itsSourceInput->InputValid()      ||
			 !itsConstructorInput->InputValid() ||
			 !itsDestructorInput->InputValid()  ||
			 !itsFunctionInput->InputValid()));
}

