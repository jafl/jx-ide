/******************************************************************************
 TabWidthDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#include "TabWidthDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TabWidthDialog::TabWidthDialog
	(
	const JSize tabWidth
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(tabWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TabWidthDialog::~TabWidthDialog()
{
}

/******************************************************************************
 GetTabCharCount

 ******************************************************************************/

JSize
TabWidthDialog::GetTabCharCount()
	const
{
	JInteger value;
	itsTabWidthInput->GetValue(&value);
	return value;
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
TabWidthDialog::BuildWindow
	(
	const JSize tabWidth
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 180,90, JGetString("WindowTitle::TabWidthDialog::JXLayout"));

	auto* tabWidthLabel =
		jnew JXStaticText(JGetString("tabWidthLabel::TabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 100,20);
	tabWidthLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,60, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::TabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,60, 60,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::TabWidthDialog::JXLayout"));

	itsTabWidthInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,20, 40,20);
	itsTabWidthInput->SetLowerLimit(1);

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsTabWidthInput->SetValue(tabWidth);
}
