/******************************************************************************
 EditProjPrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EditProjPrefsDialog.h"
#include "ProjectDocument.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

EditProjPrefsDialog::EditProjPrefsDialog
	(
	const bool							reopenTextFiles,
	const bool							doubleSpaceCompile,
	const bool							rebuildMakefileDaily,
	const ProjectTable::DropFileAction	dropFileAction
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(reopenTextFiles, doubleSpaceCompile, rebuildMakefileDaily,
				dropFileAction);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditProjPrefsDialog::~EditProjPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditProjPrefsDialog::BuildWindow
	(
	const bool							reopenTextFiles,
	const bool							doubleSpaceCompile,
	const bool							rebuildMakefileDaily,
	const ProjectTable::DropFileAction	dropFileAction
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 360,310, JGetString("WindowTitle::EditProjPrefsDialog::JXLayout"));

	itsReopenTextFilesCB =
		jnew JXTextCheckbox(JGetString("itsReopenTextFilesCB::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 300,20);

	itsDoubleSpaceCB =
		jnew JXTextCheckbox(JGetString("itsDoubleSpaceCB::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 300,20);

	itsRebuildMakefileDailyCB =
		jnew JXTextCheckbox(JGetString("itsRebuildMakefileDailyCB::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 300,20);

	auto* dndPathLabel =
		jnew JXStaticText(JGetString("dndPathLabel::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,120, 330,20);
	dndPathLabel->SetToLabel(false);

	itsDropFileActionRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,150, 204,114);

	auto* absolutePathRB =
		jnew JXTextRadioButton(ProjectTable::kAbsolutePath, JGetString("absolutePathRB::EditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 180,20);
	assert( absolutePathRB != nullptr );

	auto* projectRelativeLabel =
		jnew JXTextRadioButton(ProjectTable::kProjectRelative, JGetString("projectRelativeLabel::EditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 180,20);
	assert( projectRelativeLabel != nullptr );

	auto* homeDirLabel =
		jnew JXTextRadioButton(ProjectTable::kHomeRelative, JGetString("homeDirLabel::EditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 180,20);
	assert( homeDirLabel != nullptr );

	auto* dndAskLabel =
		jnew JXTextRadioButton(ProjectTable::kDropAskPathType, JGetString("dndAskLabel::EditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 180,20);
	assert( dndAskLabel != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,280, 70,20);
	assert( cancelButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 140,280, 70,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::EditProjPrefsDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 249,279, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::EditProjPrefsDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsReopenTextFilesCB->SetState(reopenTextFiles);
	itsDoubleSpaceCB->SetState(doubleSpaceCompile);
	itsRebuildMakefileDailyCB->SetState(rebuildMakefileDaily);
	itsDropFileActionRG->SelectItem(dropFileAction);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditProjPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("ProjectHelp-Prefs");
	}

	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateSettings

 ******************************************************************************/

void
EditProjPrefsDialog::UpdateSettings()
{
	GetApplication()->DisplayBusyCursor();

	const bool reopenTextFiles      = itsReopenTextFilesCB->IsChecked();
	const bool doubleSpaceCompile   = itsDoubleSpaceCB->IsChecked();
	const bool rebuildMakefileDaily = itsRebuildMakefileDailyCB->IsChecked();

	const auto dropFileAction =
		(ProjectTable::DropFileAction) itsDropFileActionRG->GetSelectedItem();

	for (auto* doc : *GetDocumentManager()->GetProjectDocList())
	{
		doc->SetProjectPrefs(reopenTextFiles, doubleSpaceCompile,
							 rebuildMakefileDaily, dropFileAction);
	}
}
