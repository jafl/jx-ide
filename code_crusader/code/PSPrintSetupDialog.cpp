/******************************************************************************
 PSPrintSetupDialog.cpp

	BASE CLASS = JXPSPrintSetupDialog

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "PSPrintSetupDialog.h"
#include "PSPrinter.h"
#include "PTPrinter.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXFontSizeMenu.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

PSPrintSetupDialog*
PSPrintSetupDialog::Create
	(
	const JXPSPrinter::Destination	dest,
	const JString&					printCmd,
	const JString&					fileName,
	const bool						collate,
	const bool						bw,
	const JSize						fontSize,
	const bool						printHeader
	)
{
	auto* dlog = jnew PSPrintSetupDialog;
	assert( dlog != nullptr );
	dlog->BuildWindow(dest, printCmd, fileName, collate, bw, fontSize, printHeader);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

PSPrintSetupDialog::PSPrintSetupDialog()
	:
	JXPSPrintSetupDialog()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PSPrintSetupDialog::~PSPrintSetupDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
PSPrintSetupDialog::BuildWindow
	(
	const JXPSPrinter::Destination	dest,
	const JString&					printCmd,
	const JString&					fileName,
	const bool						collate,
	const bool						bw,
	const JSize						fontSize,
	const bool						printHeader
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,300, JString::empty);
	assert( window != nullptr );

	auto* printCmdLabel =
		jnew JXStaticText(JGetString("printCmdLabel::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,70, 90,19);
	assert( printCmdLabel != nullptr );
	printCmdLabel->SetToLabel();

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,270, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::PSPrintSetupDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,270, 70,20);
	assert( cancelButton != nullptr );

	auto* destinationLabel =
		jnew JXStaticText(JGetString("destinationLabel::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,30, 80,20);
	assert( destinationLabel != nullptr );
	destinationLabel->SetToLabel();

	auto* destination =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 139,39);
	assert( destination != nullptr );

	auto* printerRB =
		jnew JXTextRadioButton(1, JGetString("printerRB::PSPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 5,8, 70,20);
	assert( printerRB != nullptr );
	printerRB->SetShortcuts(JGetString("printerRB::PSPrintSetupDialog::shortcuts::JXLayout"));

	auto* fileRB =
		jnew JXTextRadioButton(2, JGetString("fileRB::PSPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 75,8, 50,20);
	assert( fileRB != nullptr );
	fileRB->SetShortcuts(JGetString("fileRB::PSPrintSetupDialog::shortcuts::JXLayout"));

	auto* printCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,70, 250,20);
	assert( printCmdInput != nullptr );

	auto* chooseFileButton =
		jnew JXTextButton(JGetString("chooseFileButton::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,90, 80,20);
	assert( chooseFileButton != nullptr );

	auto* copyCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 170,110, 40,20);
	assert( copyCount != nullptr );

	auto* bwCheckbox =
		jnew JXTextCheckbox(JGetString("bwCheckbox::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,190, 150,20);
	assert( bwCheckbox != nullptr );

	auto* firstPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,150, 40,20);
	assert( firstPageIndex != nullptr );

	auto* lastPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 290,150, 40,20);
	assert( lastPageIndex != nullptr );

	auto* printAllCB =
		jnew JXTextCheckbox(JGetString("printAllCB::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,150, 120,20);
	assert( printAllCB != nullptr );

	auto* firstPageIndexLabel =
		jnew JXStaticText(JGetString("firstPageIndexLabel::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,150, 70,20);
	assert( firstPageIndexLabel != nullptr );
	firstPageIndexLabel->SetToLabel();

	auto* lastPageIndexLabel =
		jnew JXStaticText(JGetString("lastPageIndexLabel::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,150, 20,20);
	assert( lastPageIndexLabel != nullptr );
	lastPageIndexLabel->SetToLabel();

	auto* copiesLabel =
		jnew JXStaticText(JGetString("copiesLabel::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 55,110, 115,20);
	assert( copiesLabel != nullptr );
	copiesLabel->SetToLabel();

	auto* collateCB =
		jnew JXTextCheckbox(JGetString("collateCB::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 240,110, 70,20);
	assert( collateCB != nullptr );

	itsPrintHeaderCB =
		jnew JXTextCheckbox(JGetString("itsPrintHeaderCB::PSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,190, 130,20);
	assert( itsPrintHeaderCB != nullptr );
	itsPrintHeaderCB->SetShortcuts(JGetString("itsPrintHeaderCB::PSPrintSetupDialog::shortcuts::JXLayout"));

	itsFontSizeMenu =
		jnew JXFontSizeMenu(JFontManager::GetDefaultMonospaceFontName(), JGetString("FontSizeMenuTitle::PSPrintSetupDialog"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 120,220, 170,30);
	assert( itsFontSizeMenu != nullptr );

// end JXLayout

	SetObjects(okButton, cancelButton, destination, dest,
			   printCmdLabel, printCmdInput, printCmd,
			   chooseFileButton, fileName, copyCount,
			   collateCB, collate, bwCheckbox, bw,
			   printAllCB, firstPageIndexLabel,
			   firstPageIndex, lastPageIndexLabel, lastPageIndex);

	itsFontSizeMenu->SetFontSize(fontSize);
	itsFontSizeMenu->SetToPopupChoice();

	itsPrintHeaderCB->SetState(printHeader);
}

/******************************************************************************
 SetParameters (virtual)

 ******************************************************************************/

bool
PSPrintSetupDialog::SetParameters
	(
	JXPSPrinter* p
	)
	const
{
	bool changed = JXPSPrintSetupDialog::SetParameters(p);

	auto* p1 = dynamic_cast<PSPrinter*>(p);
	if (p1 != nullptr)
	{
		const JSize fontSize   = itsFontSizeMenu->GetFontSize();
		const bool printHeader = itsPrintHeaderCB->IsChecked();

		changed = changed ||
			fontSize    != p1->GetFontSize() ||
			printHeader != GetPTTextPrinter()->WillPrintHeader();

		p1->SetFontSize(fontSize);
		GetPTTextPrinter()->ShouldPrintHeader(printHeader);
	}

	return changed;
}
