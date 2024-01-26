/******************************************************************************
 OKToOverwriteModifiedFileDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "OKToOverwriteModifiedFileDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXImageWidget.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

#include <jx-af/image/jx/jx_un_warning.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

OKToOverwriteModifiedFileDialog::OKToOverwriteModifiedFileDialog
	(
	const JString& fileName
	)
	:
	JXUNDialogBase(),
	itsCloseAction(kSaveData)
{
	const JUtf8Byte* map[] =
	{
		"name", fileName.GetBytes()
	};

	BuildWindow(JGetString("OverwriteChangedFilePrompt::OKToOverwriteModifiedFileDialog", map, sizeof(map)));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

OKToOverwriteModifiedFileDialog::~OKToOverwriteModifiedFileDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
OKToOverwriteModifiedFileDialog::BuildWindow
	(
	const JString& message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,110, JString::empty);

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( icon != nullptr );

	auto* text =
		jnew JXStaticText(JGetString("text::OKToOverwriteModifiedFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
	assert( text != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::OKToOverwriteModifiedFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 170,80, 60,20);
	assert( cancelButton != nullptr );

	auto* saveButton =
		jnew JXTextButton(JGetString("saveButton::OKToOverwriteModifiedFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,80, 60,20);
	assert( saveButton != nullptr );
	saveButton->SetShortcuts(JGetString("saveButton::OKToOverwriteModifiedFileDialog::shortcuts::JXLayout"));

	itsCompareButton =
		jnew JXTextButton(JGetString("itsCompareButton::OKToOverwriteModifiedFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,80, 60,20);
	assert( itsCompareButton != nullptr );
	itsCompareButton->SetShortcuts(JGetString("itsCompareButton::OKToOverwriteModifiedFileDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::OKToOverwriteModifiedFileDialog"));
	SetButtons(saveButton, cancelButton);
	ListenTo(itsCompareButton, std::function([this](const JXButton::Pushed&)
	{
		itsCloseAction = kCompareData;
		EndDialog(true);
	}));

	Init(window, text, message);
}
