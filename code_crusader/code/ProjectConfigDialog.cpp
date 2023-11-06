/******************************************************************************
 ProjectConfigDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "ProjectConfigDialog.h"
#include "sharedUtil.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXCSFDialogBase.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ProjectConfigDialog::ProjectConfigDialog
	(
	const BuildManager::MakefileMethod	method,
	const JString&						targetName,
	const JString&						depListExpr,
	const JString&						updateMakefileCmd,
	const JString&						subProjectBuildCmd
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(method, targetName, depListExpr, updateMakefileCmd, subProjectBuildCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ProjectConfigDialog::~ProjectConfigDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
ProjectConfigDialog::BuildWindow
	(
	const BuildManager::MakefileMethod	method,
	const JString&						targetName,
	const JString&						depListExpr,
	const JString&						updateMakefileCmd,
	const JString&						subProjectBuildCmd
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 410,380, JString::empty);

	itsMethodRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,10, 292,106);
	assert( itsMethodRG != nullptr );

	itsTargetName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,130, 280,20);
	assert( itsTargetName != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,350, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 280,350, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::ProjectConfigDialog::shortcuts::JXLayout"));

	auto* targetNameLabel =
		jnew JXStaticText(JGetString("targetNameLabel::ProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 90,20);
	assert( targetNameLabel != nullptr );
	targetNameLabel->SetToLabel();

	auto* makemakeLabel =
		jnew JXTextRadioButton(BuildManager::kMakemake, JGetString("makemakeLabel::ProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 270,20);
	assert( makemakeLabel != nullptr );

	itsDepListExpr =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,180, 370,20);
	assert( itsDepListExpr != nullptr );

	auto* dependenciesLabel =
		jnew JXStaticText(JGetString("dependenciesLabel::ProjectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,160, 370,20);
	assert( dependenciesLabel != nullptr );
	dependenciesLabel->SetToLabel();

	auto* gmakeLabel =
		jnew JXTextRadioButton(BuildManager::kQMake, JGetString("gmakeLabel::ProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 270,20);
	assert( gmakeLabel != nullptr );

	auto* manualLabel =
		jnew JXTextRadioButton(BuildManager::kManual, JGetString("manualLabel::ProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 270,20);
	assert( manualLabel != nullptr );

	itsUpdateMakefileCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,230, 370,20);
	assert( itsUpdateMakefileCmd != nullptr );

	auto* updateMakefileLabel =
		jnew JXStaticText(JGetString("updateMakefileLabel::ProjectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,210, 370,20);
	assert( updateMakefileLabel != nullptr );
	updateMakefileLabel->SetToLabel();

	itsSubProjectBuildCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,280, 370,20);
	assert( itsSubProjectBuildCmd != nullptr );

	auto* subprojLabel =
		jnew JXStaticText(JGetString("subprojLabel::ProjectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,260, 370,20);
	assert( subprojLabel != nullptr );
	subprojLabel->SetToLabel();

	auto* configInstrText =
		jnew JXStaticText(JGetString("configInstrText::ProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 20,315, 370,25);
	assert( configInstrText != nullptr );
	configInstrText->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::ProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 165,350, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::ProjectConfigDialog::shortcuts::JXLayout"));

	auto* cmakeLabel =
		jnew JXTextRadioButton(BuildManager::kCMake, JGetString("cmakeLabel::ProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 270,20);
	assert( cmakeLabel != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::ProjectConfigDialog"));
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	window->AdjustSize(0, configInstrText->GetBoundsHeight() - configInstrText->GetFrameHeight());

	itsCurrentMethod = method;
	itsMethodRG->SelectItem(method);

	itsTargetName->GetText()->SetText(targetName);
	itsTargetName->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);

	itsDepListExpr->GetText()->SetText(depListExpr);
	itsDepListExpr->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);

	itsUpdateMakefileCmd->GetText()->SetText(updateMakefileCmd);
	itsUpdateMakefileCmd->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	itsUpdateMakefileCmd->SetIsRequired();

	itsSubProjectBuildCmd->GetText()->SetText(subProjectBuildCmd);
	itsSubProjectBuildCmd->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	itsSubProjectBuildCmd->SetIsRequired();

	UpdateDisplay();
	ListenTo(itsMethodRG);
}

/******************************************************************************
 GetConfig

 ******************************************************************************/

void
ProjectConfigDialog::GetConfig
	(
	BuildManager::MakefileMethod*	method,
	JString*						targetName,
	JString*						depListExpr,
	JString*						updateMakefileCmd,
	JString*						subProjectBuildCmd
	)
	const
{
	*method             = (BuildManager::MakefileMethod) itsMethodRG->GetSelectedItem();
	*targetName         = itsTargetName->GetText()->GetText();
	*depListExpr        = itsDepListExpr->GetText()->GetText();
	*updateMakefileCmd  = itsUpdateMakefileCmd->GetText()->GetText();
	*subProjectBuildCmd = itsSubProjectBuildCmd->GetText()->GetText();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
ProjectConfigDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsMethodRG && message.Is(JXRadioGroup::kSelectionChanged))
	{
		UpdateDisplay();
	}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("ProjectHelp-Config");
	}
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
ProjectConfigDialog::UpdateDisplay()
{
	const auto method =
		(BuildManager::MakefileMethod) itsMethodRG->GetSelectedItem();

	JString newCmd;
	if (BuildManager::UpdateMakeDependCmd(itsCurrentMethod, method, &newCmd))
	{
		itsUpdateMakefileCmd->GetText()->SetText(newCmd);
	}
	itsCurrentMethod = method;

	if (method == BuildManager::kManual)
	{
		itsTargetName->Deactivate();
		itsTargetName->SetIsRequired(false);
		itsDepListExpr->Deactivate();
		itsUpdateMakefileCmd->Focus();
	}
	else if (method == BuildManager::kQMake)
	{
		itsTargetName->Activate();
		itsTargetName->SetIsRequired(true);
		itsTargetName->Focus();
		itsDepListExpr->Deactivate();
	}
	else	// kCMake or anything else
	{
		itsTargetName->Activate();
		itsTargetName->SetIsRequired(true);
		itsTargetName->Focus();
		itsDepListExpr->Activate();
	}
}
