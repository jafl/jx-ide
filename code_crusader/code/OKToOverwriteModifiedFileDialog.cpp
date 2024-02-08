/******************************************************************************
 OKToOverwriteModifiedFileDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "OKToOverwriteModifiedFileDialog.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXImageWidget.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

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

	auto* window = jnew JXWindow(this, 330,110, JGetString("WindowTitle::OKToOverwriteModifiedFileDialog::JXLayout"));

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
#ifndef _H_jx_af_image_jx_jx_un_warning
#define _H_jx_af_image_jx_jx_un_warning
#include <jx-af/image/jx/jx_un_warning.xpm>
#endif
	icon->SetImage(GetDisplay()->GetImageCache()->GetImage(jx_un_warning), false);

	auto* text =
		jnew JXStaticText(JString::empty, true, false, false, nullptr, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
	assert( text != nullptr );

	itsCompareButton =
		jnew JXTextButton(JGetString("itsCompareButton::OKToOverwriteModifiedFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,80, 60,20);
	itsCompareButton->SetShortcuts(JGetString("itsCompareButton::shortcuts::OKToOverwriteModifiedFileDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::OKToOverwriteModifiedFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 170,80, 60,20);
	assert( cancelButton != nullptr );

	auto* saveButton =
		jnew JXTextButton(JGetString("saveButton::OKToOverwriteModifiedFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 249,79, 62,22);
	saveButton->SetShortcuts(JGetString("saveButton::shortcuts::OKToOverwriteModifiedFileDialog::JXLayout"));

// end JXLayout

	SetButtons(saveButton, cancelButton);

	ListenTo(itsCompareButton, std::function([this](const JXButton::Pushed&)
	{
		itsCloseAction = kCompareData;
		EndDialog(true);
	}));

	Init(window, text, message);
}
