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

	auto* window = jnew JXWindow(this, 360,160, JString::empty);
	assert( window != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,130, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,130, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::SubprojectConfigDialog::shortcuts::JXLayout"));

	auto* fileLabel =
		jnew JXStaticText(JGetString("fileLabel::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,60, 70,20);
	assert( fileLabel != nullptr );
	fileLabel->SetToLabel();

	itsShouldBuildCB =
		jnew JXTextCheckbox(JGetString("itsShouldBuildCB::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 230,20);
	assert( itsShouldBuildCB != nullptr );
	itsShouldBuildCB->SetShortcuts(JGetString("itsShouldBuildCB::SubprojectConfigDialog::shortcuts::JXLayout"));

	itsSubProjName =
		jnew ProjectFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 90,60, 200,20);
	assert( itsSubProjName != nullptr );

	itsChooseFileButton =
		jnew JXTextButton(JGetString("itsChooseFileButton::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 290,60, 50,20);
	assert( itsChooseFileButton != nullptr );
	itsChooseFileButton->SetShortcuts(JGetString("itsChooseFileButton::SubprojectConfigDialog::shortcuts::JXLayout"));

	itsIncludeInDepListCB =
		jnew JXTextCheckbox(JGetString("itsIncludeInDepListCB::SubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 250,20);
	assert( itsIncludeInDepListCB != nullptr );
	itsIncludeInDepListCB->SetShortcuts(JGetString("itsIncludeInDepListCB::SubprojectConfigDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SubprojectConfigDialog"));
	SetButtons(okButton, cancelButton);

	itsSubProjName->GetText()->SetText(subProjName);
	itsSubProjName->SetBasePath(basePath);
	itsSubProjName->ShouldAllowInvalidFile();

	if ((supervisor->GetBuildManager())->GetMakefileMethod() == BuildManager::kMakemake)
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
