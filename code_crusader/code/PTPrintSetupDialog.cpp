/******************************************************************************
 PTPrintSetupDialog.cpp

	BASE CLASS = JXPTPrintSetupDialog

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "PTPrintSetupDialog.h"
#include "PTPrinter.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

PTPrintSetupDialog*
PTPrintSetupDialog::Create
	(
	const JXPTPrinter::Destination	dest,
	const JString&					printCmd,
	const JString&					fileName,
	const bool					printLineNumbers,
	const bool					printHeader
	)
{
	auto* dlog = jnew PTPrintSetupDialog;
	assert( dlog != nullptr );
	dlog->BuildWindow(dest, printCmd, fileName, printLineNumbers, printHeader);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

PTPrintSetupDialog::PTPrintSetupDialog()
	:
	JXPTPrintSetupDialog()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PTPrintSetupDialog::~PTPrintSetupDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
PTPrintSetupDialog::BuildWindow
	(
	const JXPTPrinter::Destination	dest,
	const JString&					printCmd,
	const JString&					fileName,
	const bool					printLineNumbers,
	const bool					printHeader
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,240, JString::empty);
	assert( window != nullptr );

	auto* printCmdLabel =
		jnew JXStaticText(JGetString("printCmdLabel::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,70, 90,19);
	assert( printCmdLabel != nullptr );
	printCmdLabel->SetToLabel();

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,210, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::PTPrintSetupDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,210, 70,20);
	assert( cancelButton != nullptr );

	auto* destinationLabel =
		jnew JXStaticText(JGetString("destinationLabel::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,30, 80,20);
	assert( destinationLabel != nullptr );
	destinationLabel->SetToLabel();

	auto* destination =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 139,39);
	assert( destination != nullptr );

	auto* printerRB =
		jnew JXTextRadioButton(1, JGetString("printerRB::PTPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 5,8, 70,20);
	assert( printerRB != nullptr );
	printerRB->SetShortcuts(JGetString("printerRB::PTPrintSetupDialog::shortcuts::JXLayout"));

	auto* fileRB =
		jnew JXTextRadioButton(2, JGetString("fileRB::PTPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 75,8, 50,20);
	assert( fileRB != nullptr );
	fileRB->SetShortcuts(JGetString("fileRB::PTPrintSetupDialog::shortcuts::JXLayout"));

	auto* printCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,70, 250,20);
	assert( printCmdInput != nullptr );

	auto* chooseFileButton =
		jnew JXTextButton(JGetString("chooseFileButton::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,90, 80,20);
	assert( chooseFileButton != nullptr );
	chooseFileButton->SetShortcuts(JGetString("chooseFileButton::PTPrintSetupDialog::shortcuts::JXLayout"));

	auto* copyCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 140,120, 40,20);
	assert( copyCount != nullptr );

	auto* firstPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,170, 40,20);
	assert( firstPageIndex != nullptr );

	auto* lastPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 290,170, 40,20);
	assert( lastPageIndex != nullptr );

	auto* printAllCB =
		jnew JXTextCheckbox(JGetString("printAllCB::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,170, 120,20);
	assert( printAllCB != nullptr );

	auto* firstPageIndexLabel =
		jnew JXStaticText(JGetString("firstPageIndexLabel::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,170, 70,20);
	assert( firstPageIndexLabel != nullptr );
	firstPageIndexLabel->SetToLabel();

	auto* lastPageIndexLabel =
		jnew JXStaticText(JGetString("lastPageIndexLabel::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,170, 20,20);
	assert( lastPageIndexLabel != nullptr );
	lastPageIndexLabel->SetToLabel();

	auto* copiesLabel =
		jnew JXStaticText(JGetString("copiesLabel::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 25,120, 115,20);
	assert( copiesLabel != nullptr );
	copiesLabel->SetToLabel();

	itsPrintHeaderCB =
		jnew JXTextCheckbox(JGetString("itsPrintHeaderCB::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,110, 130,20);
	assert( itsPrintHeaderCB != nullptr );
	itsPrintHeaderCB->SetShortcuts(JGetString("itsPrintHeaderCB::PTPrintSetupDialog::shortcuts::JXLayout"));

	auto* printLineNumbersCB =
		jnew JXTextCheckbox(JGetString("printLineNumbersCB::PTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,130, 130,20);
	assert( printLineNumbersCB != nullptr );

// end JXLayout

	SetObjects(okButton, cancelButton, destination, dest,
			   printCmdLabel, printCmdInput, printCmd,
			   chooseFileButton, fileName, copyCount,
			   printAllCB, firstPageIndexLabel,
			   firstPageIndex, lastPageIndexLabel, lastPageIndex,
			   printLineNumbersCB, printLineNumbers);

	itsPrintHeaderCB->SetState(printHeader);
}

/******************************************************************************
 SetParameters (virtual)

 ******************************************************************************/

bool
PTPrintSetupDialog::SetParameters
	(
	JXPTPrinter* p
	)
	const
{
	bool changed = JXPTPrintSetupDialog::SetParameters(p);

	auto* p1 = dynamic_cast<PTPrinter*>(p);
	if (p1 != nullptr)
	{
		const bool printHeader = itsPrintHeaderCB->IsChecked();

		changed = changed || p1->WillPrintHeader() != printHeader;
		p1->ShouldPrintHeader(printHeader);
	}

	return changed;
}
