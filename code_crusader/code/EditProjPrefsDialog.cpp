/******************************************************************************
 EditProjPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

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
	JXDialogDirector(GetApplication(), true)
{
	BuildWindow(reopenTextFiles, doubleSpaceCompile, rebuildMakefileDaily,
				dropFileAction);
	ListenTo(this);
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

	auto* window = jnew JXWindow(this, 360,310, JString::empty);
	assert( window != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,280, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 250,280, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::EditProjPrefsDialog::shortcuts::JXLayout"));

	itsReopenTextFilesCB =
		jnew JXTextCheckbox(JGetString("itsReopenTextFilesCB::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 300,20);
	assert( itsReopenTextFilesCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 140,280, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::EditProjPrefsDialog::shortcuts::JXLayout"));

	itsDropFileActionRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,150, 204,114);
	assert( itsDropFileActionRG != nullptr );

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

	auto* dndPathLabel =
		jnew JXStaticText(JGetString("dndPathLabel::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,120, 330,20);
	assert( dndPathLabel != nullptr );
	dndPathLabel->SetToLabel();

	auto* dndAskLabel =
		jnew JXTextRadioButton(ProjectTable::kAskPathType, JGetString("dndAskLabel::EditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 180,20);
	assert( dndAskLabel != nullptr );

	itsDoubleSpaceCB =
		jnew JXTextCheckbox(JGetString("itsDoubleSpaceCB::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 300,20);
	assert( itsDoubleSpaceCB != nullptr );

	itsRebuildMakefileDailyCB =
		jnew JXTextCheckbox(JGetString("itsRebuildMakefileDailyCB::EditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 300,20);
	assert( itsRebuildMakefileDailyCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditProjPrefsDialog"));
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
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			UpdateSettings();
		}
	}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		(JXGetHelpManager())->ShowSection("ProjectHelp-Prefs");
	}

	else
	{
		JXDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateSettings (private)

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

	DocumentManager* docMgr             = GetDocumentManager();
	JPtrArray<ProjectDocument>* docList = docMgr->GetProjectDocList();
	const JSize docCount = docList->GetElementCount();

	for (JIndex i=1; i<=docCount; i++)
	{
		(docList->GetElement(i))->
			SetProjectPrefs(reopenTextFiles, doubleSpaceCompile,
							rebuildMakefileDaily, dropFileAction);
	}
}
