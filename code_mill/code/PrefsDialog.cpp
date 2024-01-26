/******************************************************************************
 PrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2002 by Glenn W. Bach.

 ******************************************************************************/

#include "PrefsDialog.h"
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PrefsDialog::PrefsDialog
	(
	const JString& header,
	const JString& source,
	const JString& constructor,
	const JString& destructor,
	const JString& function
	)
	:
	JXModalDialogDirector()
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

	auto* window = jnew JXWindow(this, 1001,530, JString::empty);

	auto* headerCommentLabel =
		jnew JXStaticText(JGetString("headerCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 470,20);
	headerCommentLabel->SetToLabel(false);

	auto* ctorCommentLabel =
		jnew JXStaticText(JGetString("ctorCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 510,20, 470,20);
	ctorCommentLabel->SetToLabel(false);

	auto* dtorCommentLabel =
		jnew JXStaticText(JGetString("dtorCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 510,175, 470,20);
	dtorCommentLabel->SetToLabel(false);

	auto* sourceCommentLabel =
		jnew JXStaticText(JGetString("sourceCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,250, 470,20);
	sourceCommentLabel->SetToLabel(false);

	auto* fnCommentLabel =
		jnew JXStaticText(JGetString("fnCommentLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 510,330, 470,20);
	fnCommentLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 280,500, 70,20);
	cancelButton->SetShortcuts(JGetString("cancelButton::shortcuts::PrefsDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 660,500, 70,20);
	assert( okButton != nullptr );

	itsHeaderInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 470,200);
	itsHeaderInput->SetIsRequired();

	itsSourceInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,270, 470,200);
	itsSourceInput->SetIsRequired();

	itsConstructorInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 510,40, 470,120);
	itsConstructorInput->SetIsRequired();

	itsDestructorInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 510,195, 470,120);
	itsDestructorInput->SetIsRequired();

	itsFunctionInput =
		jnew JXInputField(false, true, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 510,350, 470,120);
	itsFunctionInput->SetIsRequired();

// end JXLayout

	SetButtons(okButton, cancelButton);

	const JFont f = JFontManager::GetDefaultMonospaceFont();
	itsHeaderInput->GetText()->SetDefaultFont(f);
	itsSourceInput->GetText()->SetDefaultFont(f);
	itsConstructorInput->GetText()->SetDefaultFont(f);
	itsDestructorInput->GetText()->SetDefaultFont(f);
	itsFunctionInput->GetText()->SetDefaultFont(f);

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
