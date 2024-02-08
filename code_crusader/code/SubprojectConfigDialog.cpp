/******************************************************************************
 SubprojectConfigDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "SubprojectConfigDialog.h"
#include "ProjectFileInput.h"
#include "ProjectDocument.h"
#include "ChooseRelativeFileDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SubprojectConfigDialog::SubprojectConfigDialog
	(
	ProjectDocument*	supervisor,
	const bool			includeInDepList,
	const JString&		subProjName,
	const bool			shouldBuild
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(supervisor, includeInDepList, subProjName, shouldBuild,
				supervisor->GetFilePath());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SubprojectConfigDialog::~SubprojectConfigDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SubprojectConfigDialog::BuildWindow
	(
	ProjectDocument*	supervisor,
	const bool			includeInDepList,
	const JString&		subProjName,
	const bool			shouldBuild,
	const JString&		basePath
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 360,160, JGetString("WindowTitle::SubprojectConfigDialog::JXLayout"));

	itsIncludeInDepListCB =
		jnew JXTextCheckbox(JGetString("itsIncludeInDepListCB::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 250,20);
	itsIncludeInDepListCB->SetShortcuts(JGetString("itsIncludeInDepListCB::shortcuts::SubprojectConfigDialog::JXLayout"));

	auto* fileLabel =
		jnew JXStaticText(JGetString("fileLabel::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 70,20);
	fileLabel->SetToLabel(false);

	itsSubProjName =
		jnew ProjectFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 90,60, 200,20);

	itsChooseFileButton =
		jnew JXTextButton(JGetString("itsChooseFileButton::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,60, 50,20);
	itsChooseFileButton->SetShortcuts(JGetString("itsChooseFileButton::shortcuts::SubprojectConfigDialog::JXLayout"));

	itsShouldBuildCB =
		jnew JXTextCheckbox(JGetString("itsShouldBuildCB::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 230,20);
	itsShouldBuildCB->SetShortcuts(JGetString("itsShouldBuildCB::shortcuts::SubprojectConfigDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,130, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 219,129, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::SubprojectConfigDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsSubProjName->GetText()->SetText(subProjName);
	itsSubProjName->SetBasePath(basePath);
	itsSubProjName->ShouldAllowInvalidFile();

	if (supervisor->GetBuildManager()->GetMakefileMethod() == BuildManager::kMakemake)
	{
		itsIncludeInDepListCB->SetState(includeInDepList);
		itsShouldBuildCB->SetState(shouldBuild);

		UpdateDisplay();
	}
	else
	{
		itsIncludeInDepListCB->SetState(false);
		itsShouldBuildCB->SetState(false);

		UpdateDisplay();	// before other deactivations

		itsIncludeInDepListCB->Deactivate();
		itsShouldBuildCB->Deactivate();
	}

	ListenTo(itsChooseFileButton);
	ListenTo(itsIncludeInDepListCB);
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
SubprojectConfigDialog::UpdateDisplay()
{
	itsShouldBuildCB->SetActive(itsIncludeInDepListCB->IsChecked());
	if (!itsIncludeInDepListCB->IsChecked())
	{
		itsShouldBuildCB->SetState(false);
	}
}

/******************************************************************************
 GetConfig

 ******************************************************************************/

void
SubprojectConfigDialog::GetConfig
	(
	bool*		includeInDepList,
	JString*	subProjName,
	bool*		shouldBuild
	)
	const
{
	*includeInDepList = itsIncludeInDepListCB->IsChecked();

	*subProjName = itsSubProjName->GetText()->GetText();
	*shouldBuild = itsShouldBuildCB->IsChecked();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SubprojectConfigDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseFileButton && message.Is(JXButton::kPushed))
	{
		ChooseProjectFile();
	}
	else if (sender == itsIncludeInDepListCB && message.Is(JXCheckbox::kPushed))
	{
		UpdateDisplay();
	}
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ChooseProjectFile (private)

 ******************************************************************************/

void
SubprojectConfigDialog::ChooseProjectFile()
{
	auto* dlog =
		ChooseRelativeFileDialog::Create(
			ProjectTable::CalcPathType(itsSubProjName->GetText()->GetText()),
			JXChooseFileDialog::kSelectSingleFile,
			itsSubProjName->GetTextForChooseFile(),
			JString::empty, JGetString("ChooseProjectFile::SubprojectConfigDialog"));

	if (dlog->DoDialog())
	{
		const JString fullName = dlog->GetFullName();
		if (ProjectDocument::CanReadFile(fullName) == JXFileDocument::kFileReadable)
		{
			JString basePath;
			const bool ok = itsSubProjName->GetBasePath(&basePath);
			assert( ok );

			itsSubProjName->GetText()->SetText(
				ProjectTable::ConvertToRelativePath(fullName, basePath, dlog->GetPathType()));
		}
		else
		{
			JGetUserNotification()->ReportError(JGetString("NotProjectFile::SubprojectConfigDialog"));
		}
	}
}
