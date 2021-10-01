/******************************************************************************
 EditCPPMacroDialog.cpp

	BASE CLASS = JXDialogDirector

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
	JXDirector*				supervisor,
	const CPreprocessor&	cpp
	)
	:
	JXDialogDirector(supervisor, true),
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

	auto* window = jnew JXWindow(this, 370,290, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 250,200);
	assert( scrollbarSet != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,260, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 260,260, 70,20);
	assert( okButton != nullptr );

	auto* addMacroButton =
		jnew JXTextButton(JGetString("addMacroButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,50, 60,20);
	assert( addMacroButton != nullptr );
	addMacroButton->SetShortcuts(JGetString("addMacroButton::EditCPPMacroDialog::shortcuts::JXLayout"));

	auto* removeMacroButton =
		jnew JXTextButton(JGetString("removeMacroButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,80, 60,20);
	assert( removeMacroButton != nullptr );
	removeMacroButton->SetShortcuts(JGetString("removeMacroButton::EditCPPMacroDialog::shortcuts::JXLayout"));

	auto* loadMacroButton =
		jnew JXTextButton(JGetString("loadMacroButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,130, 60,20);
	assert( loadMacroButton != nullptr );
	loadMacroButton->SetShortcuts(JGetString("loadMacroButton::EditCPPMacroDialog::shortcuts::JXLayout"));

	auto* saveMacroButton =
		jnew JXTextButton(JGetString("saveMacroButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,160, 60,20);
	assert( saveMacroButton != nullptr );
	saveMacroButton->SetShortcuts(JGetString("saveMacroButton::EditCPPMacroDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditCPPMacroDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 150,260, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::EditCPPMacroDialog::shortcuts::JXLayout"));

	auto* colHeaderEncl =
		jnew JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 250,20);
	assert( colHeaderEncl != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditCPPMacroDialog"));
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	UseModalPlacement(false);
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsTable =
		jnew CPPMacroTable(cpp, addMacroButton, removeMacroButton,
							loadMacroButton, saveMacroButton,
							scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTable != nullptr );

	auto* colHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, colHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 10,10);
	assert( colHeader != nullptr );
	colHeader->FitToEnclosure();
	colHeader->TurnOnColResizing(20);
	itsTable->SetColTitles(colHeader);
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
		(JXGetHelpManager())->ShowSection("CTreeHelp#CPP");
	}
	else
	{
		JXDialogDirector::Receive(sender, message);
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
		(JXDialogDirector::OKToDeactivate() && itsTable->ContentsValid());
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
