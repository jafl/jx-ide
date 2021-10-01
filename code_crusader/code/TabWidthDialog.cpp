/******************************************************************************
 TabWidthDialog.cpp

	BASE CLASS = JXDialogDirector

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
	JXWindowDirector*	supervisor,
	const JSize			tabWidth
	)
	:
	JXDialogDirector(supervisor, true)
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

	auto* window = jnew JXWindow(this, 180,90, JString::empty);
	assert( window != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,60, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::TabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,60, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::TabWidthDialog::shortcuts::JXLayout"));

	itsTabWidthInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,20, 40,20);
	assert( itsTabWidthInput != nullptr );

	auto* tabWidthLabel =
		jnew JXStaticText(JGetString("tabWidthLabel::TabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 100,20);
	assert( tabWidthLabel != nullptr );
	tabWidthLabel->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TabWidthDialog"));
	SetButtons(okButton, cancelButton);

	itsTabWidthInput->SetValue(tabWidth);
	itsTabWidthInput->SetLowerLimit(1);
}
