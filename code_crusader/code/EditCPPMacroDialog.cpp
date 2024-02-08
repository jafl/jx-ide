/******************************************************************************
 EditCPPMacroDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EditCPPMacroDialog.h"
#include "CPPMacroTable.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

EditCPPMacroDialog::EditCPPMacroDialog
	(
	const CPreprocessor& cpp
	)
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsManager(), kEditCPPMacroDialogID)
{
	BuildWindow(cpp);
	JPrefObject::ReadPrefs();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditCPPMacroDialog::~EditCPPMacroDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 UpdateMacros

	Returns true if anything changed.

 ******************************************************************************/

bool
EditCPPMacroDialog::UpdateMacros
	(
	CPreprocessor* cpp
	)
	const
{
	return itsTable->UpdateMacros(cpp);
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditCPPMacroDialog::BuildWindow
	(
	const CPreprocessor& cpp
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,290, JGetString("WindowTitle::EditCPPMacroDialog::JXLayout"));

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 250,220);
	assert( scrollbarSet != nullptr );

	auto* addMacroButton =
		jnew JXTextButton(JGetString("addMacroButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,50, 60,20);
	addMacroButton->SetShortcuts(JGetString("addMacroButton::shortcuts::EditCPPMacroDialog::JXLayout"));

	auto* removeMacroButton =
		jnew JXTextButton(JGetString("removeMacroButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,80, 60,20);
	removeMacroButton->SetShortcuts(JGetString("removeMacroButton::shortcuts::EditCPPMacroDialog::JXLayout"));

	auto* loadMacroButton =
		jnew JXTextButton(JGetString("loadMacroButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,130, 60,20);
	loadMacroButton->SetShortcuts(JGetString("loadMacroButton::shortcuts::EditCPPMacroDialog::JXLayout"));

	auto* saveMacroButton =
		jnew JXTextButton(JGetString("saveMacroButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,160, 60,20);
	saveMacroButton->SetShortcuts(JGetString("saveMacroButton::shortcuts::EditCPPMacroDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,260, 70,20);
	assert( cancelButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 150,260, 70,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::EditCPPMacroDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 260,260, 70,20);
	assert( okButton != nullptr );

	itsTable =
		jnew CPPMacroTable(cpp, addMacroButton, removeMacroButton, loadMacroButton, saveMacroButton, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 250,200);

	auto* colHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 250,20);
	assert( colHeader != nullptr );

// end JXLayout

	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	colHeader->TurnOnColResizing(20);
	itsTable->SetColTitles(colHeader);

	ListenTo(itsHelpButton);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
EditCPPMacroDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("CTreeHelp#CPP");
	}
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Check that the active object is willing to unfocus.

 ******************************************************************************/

bool
EditCPPMacroDialog::OKToDeactivate()
{
	return Cancelled() ||
		(JXModalDialogDirector::OKToDeactivate() && itsTable->ContentsValid());
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
EditCPPMacroDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
	{
		JXWindow* window = GetWindow();
		window->ReadGeometry(input);
		window->Deiconify();

		itsTable->ReadSetup(input);
	}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
EditCPPMacroDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsTable->WriteSetup(output);
}
