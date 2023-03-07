/******************************************************************************
 EditMiscPrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EditMiscPrefsDialog.h"
#include "ProjectDocument.h"
#include "MDIServer.h"
#include "globals.h"
#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXAtLeastOneCBGroup.h>
#include <jx-af/jx/JXMenu.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

EditMiscPrefsDialog::EditMiscPrefsDialog()
	:
	JXModalDialogDirector()
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditMiscPrefsDialog::~EditMiscPrefsDialog()
{
}

/******************************************************************************
 UpdateSettings

 ******************************************************************************/

void
EditMiscPrefsDialog::UpdateSettings()
{
	JXMenu::SetDisplayStyle(itsMacStyleCB->IsChecked() ?
							JXMenu::kMacintoshStyle : JXMenu::kWindowsStyle);
	JTextEditor::ShouldCopyWhenSelect(itsCopyWhenSelectCB->IsChecked());
	JXTEBase::MiddleButtonShouldPaste(itsMiddleButtonPasteCB->IsChecked());
	JXWindow::ShouldFocusFollowCursorInDock(itsFocusInDockCB->IsChecked());
	JXFileDocument::ShouldAskOKToClose(itsCloseUnsavedCB->IsChecked());
	ProjectDocument::ShouldAskOKToOpenOldVersion(itsOpenOldProjCB->IsChecked());

	GetDocumentManager()->SetWarnings(itsSaveAllCB->IsChecked(),
										  itsCloseAllCB->IsChecked());

	GetApplication()->SetWarnings(itsQuitCB->IsChecked());

	MDIServer* mdi;
	if (GetMDIServer(&mdi))
	{
		mdi->SetStartupOptions(itsNewEditorCB->IsChecked(),
							   itsNewProjectCB->IsChecked(),
							   itsReopenLastCB->IsChecked(),
							   itsChooseFileCB->IsChecked());
	}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditMiscPrefsDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,390, JString::empty);
	assert( window != nullptr );

	auto* warnTitle =
		jnew JXStaticText(JGetString("warnTitle::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,170, 80,20);
	assert( warnTitle != nullptr );
	warnTitle->SetToLabel();

	itsSaveAllCB =
		jnew JXTextCheckbox(JGetString("itsSaveAllCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,260, 120,20);
	assert( itsSaveAllCB != nullptr );

	itsCloseAllCB =
		jnew JXTextCheckbox(JGetString("itsCloseAllCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,290, 120,20);
	assert( itsCloseAllCB != nullptr );

	itsQuitCB =
		jnew JXTextCheckbox(JGetString("itsQuitCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,320, 120,20);
	assert( itsQuitCB != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,360, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,360, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::EditMiscPrefsDialog::shortcuts::JXLayout"));

	auto* noArgsTitle =
		jnew JXStaticText(JGetString("noArgsTitle::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 180,20);
	assert( noArgsTitle != nullptr );
	noArgsTitle->SetToLabel();

	itsNewEditorCB =
		jnew JXTextCheckbox(JGetString("itsNewEditorCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,200, 150,20);
	assert( itsNewEditorCB != nullptr );

	itsNewProjectCB =
		jnew JXTextCheckbox(JGetString("itsNewProjectCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,230, 150,20);
	assert( itsNewProjectCB != nullptr );

	itsChooseFileCB =
		jnew JXTextCheckbox(JGetString("itsChooseFileCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,310, 150,20);
	assert( itsChooseFileCB != nullptr );

	itsReopenLastCB =
		jnew JXTextCheckbox(JGetString("itsReopenLastCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,260, 150,20);
	assert( itsReopenLastCB != nullptr );

	auto* firstTimeHint =
		jnew JXStaticText(JGetString("firstTimeHint::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,280, 100,20);
	assert( firstTimeHint != nullptr );
	firstTimeHint->SetToLabel();

	itsCopyWhenSelectCB =
		jnew JXTextCheckbox(JGetString("itsCopyWhenSelectCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 330,20);
	assert( itsCopyWhenSelectCB != nullptr );

	itsCloseUnsavedCB =
		jnew JXTextCheckbox(JGetString("itsCloseUnsavedCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,230, 120,20);
	assert( itsCloseUnsavedCB != nullptr );

	itsMacStyleCB =
		jnew JXTextCheckbox(JGetString("itsMacStyleCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 330,20);
	assert( itsMacStyleCB != nullptr );

	auto* restartHint =
		jnew JXStaticText(JGetString("restartHint::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,40, 290,20);
	assert( restartHint != nullptr );
	restartHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	restartHint->SetToLabel();

	itsOpenOldProjCB =
		jnew JXTextCheckbox(JGetString("itsOpenOldProjCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,200, 120,20);
	assert( itsOpenOldProjCB != nullptr );

	itsFocusInDockCB =
		jnew JXTextCheckbox(JGetString("itsFocusInDockCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 330,20);
	assert( itsFocusInDockCB != nullptr );

	itsMiddleButtonPasteCB =
		jnew JXTextCheckbox(JGetString("itsMiddleButtonPasteCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 330,20);
	assert( itsMiddleButtonPasteCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditMiscPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsMacStyleCB->SetState(JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle);
	itsCopyWhenSelectCB->SetState(JTextEditor::WillCopyWhenSelect());
	itsMiddleButtonPasteCB->SetState(JXTEBase::MiddleButtonWillPaste());
	itsFocusInDockCB->SetState(JXWindow::WillFocusFollowCursorInDock());

	bool warnSave, warnClose, warnQuit;
	GetDocumentManager()->GetWarnings(&warnSave, &warnClose);
	GetApplication()->GetWarnings(&warnQuit);

	itsOpenOldProjCB->SetState(ProjectDocument::WillAskOKToOpenOldVersion());
	itsCloseUnsavedCB->SetState(JXFileDocument::WillAskOKToClose());
	itsSaveAllCB->SetState(warnSave);
	itsCloseAllCB->SetState(warnClose);
	itsQuitCB->SetState(warnQuit);

	bool newEditor  = false, newProject = false,
			 reopenLast = true,  chooseFile = false;
	MDIServer* mdi;
	if (GetMDIServer(&mdi))
	{
		mdi->GetStartupOptions(&newEditor, &newProject,
							   &reopenLast, &chooseFile);
	}

	itsNewEditorCB->SetState(newEditor);
	itsNewProjectCB->SetState(newProject);
	itsReopenLastCB->SetState(reopenLast);
	itsChooseFileCB->SetState(chooseFile);

	// bind the checkboxes so at least one remains selected

	auto* group =
		jnew JXAtLeastOneCBGroup(4, itsNewEditorCB, itsNewProjectCB,
								itsReopenLastCB, itsChooseFileCB);
	assert( group != nullptr );
}
