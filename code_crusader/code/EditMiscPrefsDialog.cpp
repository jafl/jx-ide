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

	auto* window = jnew JXWindow(this, 370,390, JGetString("WindowTitle::EditMiscPrefsDialog::JXLayout"));

	itsMacStyleCB =
		jnew JXTextCheckbox(JGetString("itsMacStyleCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 330,20);

	auto* restartHint =
		jnew JXStaticText(JGetString("restartHint::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,40, 290,20);
	restartHint->SetToLabel(false);

	itsCopyWhenSelectCB =
		jnew JXTextCheckbox(JGetString("itsCopyWhenSelectCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 330,20);

	itsMiddleButtonPasteCB =
		jnew JXTextCheckbox(JGetString("itsMiddleButtonPasteCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 330,20);

	itsFocusInDockCB =
		jnew JXTextCheckbox(JGetString("itsFocusInDockCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 330,20);

	auto* noArgsTitle =
		jnew JXStaticText(JGetString("noArgsTitle::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 180,20);
	noArgsTitle->SetToLabel(false);

	auto* warnTitle =
		jnew JXStaticText(JGetString("warnTitle::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,170, 80,20);
	warnTitle->SetToLabel(false);

	itsNewEditorCB =
		jnew JXTextCheckbox(JGetString("itsNewEditorCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,200, 150,20);

	itsOpenOldProjCB =
		jnew JXTextCheckbox(JGetString("itsOpenOldProjCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,200, 120,20);

	itsNewProjectCB =
		jnew JXTextCheckbox(JGetString("itsNewProjectCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,230, 150,20);

	itsCloseUnsavedCB =
		jnew JXTextCheckbox(JGetString("itsCloseUnsavedCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,230, 120,20);

	itsReopenLastCB =
		jnew JXTextCheckbox(JGetString("itsReopenLastCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,260, 150,20);

	itsSaveAllCB =
		jnew JXTextCheckbox(JGetString("itsSaveAllCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,260, 120,20);

	auto* firstTimeHint =
		jnew JXStaticText(JGetString("firstTimeHint::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,280, 100,20);
	firstTimeHint->SetToLabel(false);

	itsCloseAllCB =
		jnew JXTextCheckbox(JGetString("itsCloseAllCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,290, 120,20);

	itsChooseFileCB =
		jnew JXTextCheckbox(JGetString("itsChooseFileCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,310, 150,20);

	itsQuitCB =
		jnew JXTextCheckbox(JGetString("itsQuitCB::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,320, 120,20);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,360, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 230,360, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::EditMiscPrefsDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, cancelButton);

	restartHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	firstTimeHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);

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

	jnew JXAtLeastOneCBGroup(4, itsNewEditorCB, itsNewProjectCB,
							itsReopenLastCB, itsChooseFileCB);
}
