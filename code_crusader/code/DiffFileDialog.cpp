/******************************************************************************
 DiffFileDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "DiffFileDialog.h"
#include "DiffDocument.h"
#include "DiffStyleMenu.h"
#include "SVNFileInput.h"
#include "CommandManager.h"
#include "ProjectDocument.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXFileInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTabGroup.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/JXChoosePathDialog.h>
#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStringMatch.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jWebUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

static JString kDot(".");

enum
{
	kCurrentRevCmd = 1,
	kPreviousRevCmd,
	kRevisionNumberCmd,
	kRevisionDateCmd,

	// SVN

	kTrunkCmd,
	kBranchCmd,
	kTagCmd,

	// git

	kGitBranchCmd    = kTrunkCmd,
	kGit1AncestorCmd = kBranchCmd
};

inline bool
isFixedRevCmd
	(
	const JIndex cmd
	)
{
	return (cmd == kCurrentRevCmd ||
			cmd == kPreviousRevCmd);
}

inline bool
isSVNFixedRevCmd
	(
	const JIndex cmd
	)
{
	return (cmd == kCurrentRevCmd  ||
			cmd == kPreviousRevCmd ||
			cmd == kTrunkCmd);
}

// setup information

const JFileVersion kCurrentSetupVersion = 2;

	// version 2 stores itsIgnoreSpaceChangeCB, itsIgnoreBlankLinesCB
	// version 1 stores itsStayOpenCB

/******************************************************************************
 Constructor

 ******************************************************************************/

DiffFileDialog::DiffFileDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kDiffFileDialogID)
{
	itsTabIndex   = kPlainDiffTabIndex;
	itsCVSRev1Cmd = kCurrentRevCmd;
	itsCVSRev2Cmd = kCurrentRevCmd;
	itsSVNRev1Cmd = kCurrentRevCmd;
	itsSVNRev2Cmd = kCurrentRevCmd;
	itsGitRev1Cmd = kCurrentRevCmd;
	itsGitRev2Cmd = kCurrentRevCmd;

	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DiffFileDialog::~DiffFileDialog()
{
	// prefs written by DeleteGlobals()
}

/******************************************************************************
 Set file

 ******************************************************************************/

void
DiffFileDialog::SetFile1
	(
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kPlainDiffTabIndex);
	itsPlainFile1Input->GetText()->SetText(fullName);
	Activate();
	itsPlainFile1Input->Focus();
}

void
DiffFileDialog::SetFile2
	(
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kPlainDiffTabIndex);
	itsPlainFile2Input->GetText()->SetText(fullName);
	Activate();
	itsPlainFile2Input->Focus();
}

void
DiffFileDialog::SetCVSFile
	(
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kCVSDiffTabIndex);
	itsCVSFileInput->GetText()->SetText(fullName);
	Activate();
	itsCVSFileInput->Focus();
}

void
DiffFileDialog::SetSVNFile
	(
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kSVNDiffTabIndex);
	itsSVNFileInput->GetText()->SetText(fullName);
	Activate();
	itsSVNFileInput->Focus();
}

void
DiffFileDialog::SetGitFile
	(
	const JString& fullName
	)
{
	itsTabGroup->ShowTab(kGitDiffTabIndex);
	itsGitFileInput->GetText()->SetText(fullName);
	Activate();
	itsGitFileInput->Focus();
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

#include "DiffFileDialog-CVSRev1.h"
#include "DiffFileDialog-CVSRev2.h"
#include "DiffFileDialog-SVNRev1.h"
#include "DiffFileDialog-SVNRev2.h"
#include "DiffFileDialog-GitRev1.h"
#include "DiffFileDialog-GitRev2.h"

void
DiffFileDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 550,210, JGetString("WindowTitle::DiffFileDialog::JXLayout"));

	itsTabGroup =
		jnew JXTabGroup(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 530,120);
	itsTabGroup->AppendTab(JGetString("itsTabGroup::tab1::DiffFileDialog::JXLayout"));
	itsTabGroup->AppendTab(JGetString("itsTabGroup::tab2::DiffFileDialog::JXLayout"));
	itsTabGroup->AppendTab(JGetString("itsTabGroup::tab3::DiffFileDialog::JXLayout"));
	itsTabGroup->AppendTab(JGetString("itsTabGroup::tab4::DiffFileDialog::JXLayout"));

	itsSVNFileInput =
		jnew SVNFileInput(itsTabGroup->GetTabEnclosure(3),
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 330,20);

	auto* file1Label =
		jnew JXStaticText(JGetString("file1Label::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(1),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 20,20);
	file1Label->SetToLabel(false);

	itsGitChooseButton =
		jnew JXTextButton(JGetString("itsGitChooseButton::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(4),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,10, 60,20);

	itsSVNChooseButton =
		jnew JXTextButton(JGetString("itsSVNChooseButton::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(3),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,10, 60,20);

	itsCVSChooseButton =
		jnew JXTextButton(JGetString("itsCVSChooseButton::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,10, 60,20);

	itsPlainChoose1Button =
		jnew JXTextButton(JGetString("itsPlainChoose1Button::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(1),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,10, 60,20);

	itsGitSummaryCB =
		jnew JXTextCheckbox(JGetString("itsGitSummaryCB::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(4),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,10, 80,20);

	itsSVNSummaryCB =
		jnew JXTextCheckbox(JGetString("itsSVNSummaryCB::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(3),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,10, 80,20);

	itsCVSSummaryCB =
		jnew JXTextCheckbox(JGetString("itsCVSSummaryCB::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,10, 80,20);

	itsPlainOnly1StyleMenu =
		jnew DiffStyleMenu(JGetString("StyleMenuTitle::DiffDialog"), itsTabGroup->GetTabEnclosure(1),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,10, 70,20);

	itsGitRev1Menu =
		jnew JXTextMenu(JGetString("itsGitRev1Menu::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(4),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 140,20);

	itsSVNRev1Menu =
		jnew JXTextMenu(JGetString("itsSVNRev1Menu::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(3),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 140,20);

	itsCVSRev1Menu =
		jnew JXTextMenu(JGetString("itsCVSRev1Menu::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(2),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 140,20);

	auto* file2Label =
		jnew JXStaticText(JGetString("file2Label::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(1),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 20,20);
	file2Label->SetToLabel(false);

	itsGitRev2Menu =
		jnew JXTextMenu(JGetString("itsGitRev2Menu::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(4),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 140,20);

	itsSVNRev2Menu =
		jnew JXTextMenu(JGetString("itsSVNRev2Menu::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(3),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 140,20);

	itsCVSRev2Menu =
		jnew JXTextMenu(JGetString("itsCVSRev2Menu::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 140,20);

	itsPlainChoose2Button =
		jnew JXTextButton(JGetString("itsPlainChoose2Button::DiffFileDialog::JXLayout"), itsTabGroup->GetTabEnclosure(1),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,40, 60,20);

	itsPlainOnly2StyleMenu =
		jnew DiffStyleMenu(JGetString("StyleMenuTitle::DiffDialog"), itsTabGroup->GetTabEnclosure(1),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,40, 70,20);

	itsGitOnly1StyleMenu =
		jnew DiffStyleMenu(JGetString("StyleMenuTitle::DiffDialog"), itsTabGroup->GetTabEnclosure(4),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 70,20);

	itsSVNOnly1StyleMenu =
		jnew DiffStyleMenu(JGetString("StyleMenuTitle::DiffDialog"), itsTabGroup->GetTabEnclosure(3),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 70,20);

	itsCVSOnly1StyleMenu =
		jnew DiffStyleMenu(JGetString("StyleMenuTitle::DiffDialog"), itsTabGroup->GetTabEnclosure(2),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 70,20);

	itsGitOnly2StyleMenu =
		jnew DiffStyleMenu(JGetString("StyleMenuTitle::DiffDialog"), itsTabGroup->GetTabEnclosure(4),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,60, 70,20);

	itsSVNOnly2StyleMenu =
		jnew DiffStyleMenu(JGetString("StyleMenuTitle::DiffDialog"), itsTabGroup->GetTabEnclosure(3),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,60, 70,20);

	itsCVSOnly2StyleMenu =
		jnew DiffStyleMenu(JGetString("StyleMenuTitle::DiffDialog"), itsTabGroup->GetTabEnclosure(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,60, 70,20);

	itsIgnoreSpaceChangeCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreSpaceChangeCB::DiffFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 270,20);

	itsIgnoreBlankLinesCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreBlankLinesCB::DiffFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,140, 230,20);

	itsCommonStyleMenu =
		jnew DiffStyleMenu(JGetString("SharedStyleMenuTitle::DiffFileDialog"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 140,30);

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::DiffFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,175, 90,20);

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::DiffFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,175, 60,20);
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::shortcuts::DiffFileDialog::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::DiffFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,175, 60,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::DiffFileDialog::JXLayout"));

	itsViewButton =
		jnew JXTextButton(JGetString("itsViewButton::DiffFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 460,175, 60,20);
	itsViewButton->SetShortcuts(JGetString("itsViewButton::shortcuts::DiffFileDialog::JXLayout"));

	itsPlainFile1Input =
		jnew JXFileInput(itsTabGroup->GetTabEnclosure(1),
					JXWidget::kHElastic, JXWidget::kFixedTop, 30,10, 320,20);
	itsPlainFile1Input->SetIsRequired(true);
	itsPlainFile1Input->ShouldAllowInvalidFile(true);
	itsPlainFile1Input->ShouldRequireReadable(true);
	itsPlainFile1Input->ShouldRequireWritable(false);
	itsPlainFile1Input->ShouldRequireExecutable(false);

	itsPlainFile2Input =
		jnew JXFileInput(itsTabGroup->GetTabEnclosure(1),
					JXWidget::kHElastic, JXWidget::kFixedTop, 30,40, 320,20);
	itsPlainFile2Input->SetIsRequired(true);
	itsPlainFile2Input->ShouldAllowInvalidFile(true);
	itsPlainFile2Input->ShouldRequireReadable(true);
	itsPlainFile2Input->ShouldRequireWritable(false);
	itsPlainFile2Input->ShouldRequireExecutable(false);

	itsCVSFileInput =
		jnew JXFileInput(itsTabGroup->GetTabEnclosure(2),
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 330,20);
	itsCVSFileInput->SetIsRequired(true);
	itsCVSFileInput->ShouldAllowInvalidFile(true);
	itsCVSFileInput->ShouldRequireReadable(true);
	itsCVSFileInput->ShouldRequireWritable(false);
	itsCVSFileInput->ShouldRequireExecutable(false);

	itsCVSRev1Input =
		jnew JXInputField(itsTabGroup->GetTabEnclosure(2),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 140,20);

	itsCVSRev2Input =
		jnew JXInputField(itsTabGroup->GetTabEnclosure(2),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,60, 140,20);

	itsSVNRev1Input =
		jnew JXInputField(itsTabGroup->GetTabEnclosure(3),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 140,20);

	itsSVNRev2Input =
		jnew JXInputField(itsTabGroup->GetTabEnclosure(3),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,60, 140,20);

	itsGitFileInput =
		jnew JXFileInput(itsTabGroup->GetTabEnclosure(4),
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 330,20);
	itsGitFileInput->SetIsRequired(true);
	itsGitFileInput->ShouldAllowInvalidFile(true);
	itsGitFileInput->ShouldRequireReadable(true);
	itsGitFileInput->ShouldRequireWritable(false);
	itsGitFileInput->ShouldRequireExecutable(false);

	itsGitRev1Input =
		jnew JXInputField(itsTabGroup->GetTabEnclosure(4),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 140,20);

	itsGitRev2Input =
		jnew JXInputField(itsTabGroup->GetTabEnclosure(4),
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,60, 140,20);

// end JXLayout

	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

	assert( kTabCount == 4 );
	itsStyleMenu[0][0] = itsPlainOnly1StyleMenu;
	itsStyleMenu[0][1] = itsPlainOnly2StyleMenu;
	itsStyleMenu[1][0] = itsCVSOnly1StyleMenu;
	itsStyleMenu[1][1] = itsCVSOnly2StyleMenu;
	itsStyleMenu[2][0] = itsSVNOnly1StyleMenu;
	itsStyleMenu[2][1] = itsSVNOnly2StyleMenu;
	itsStyleMenu[3][0] = itsGitOnly1StyleMenu;
	itsStyleMenu[3][1] = itsGitOnly2StyleMenu;

	ListenTo(itsTabGroup->GetCardEnclosure());
	ListenTo(itsViewButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
	ListenTo(itsPlainChoose1Button);
	ListenTo(itsPlainChoose2Button);
	ListenTo(itsCVSChooseButton);
	ListenTo(itsSVNChooseButton);
	ListenTo(itsGitChooseButton);

	// plain

	itsPlainFile1Input->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsPlainFile1Input);

	itsPlainFile2Input->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsPlainFile2Input);

	itsPlainOnly1StyleMenu->SetStyle(
		JFontStyle(false, false, 0, true, JColorManager::GetRedColor()));
	itsPlainOnly2StyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetBlueColor()));
	itsCommonStyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetGrayColor(50)));

	// CVS

	itsCVSRev1Menu->SetMenuItems(kCVSRev1MenuStr);
	itsCVSRev1Menu->SetToPopupChoice(true, itsCVSRev1Cmd);
	itsCVSRev1Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCVSRev1Menu);
	ConfigureCVSRev1Menu(itsCVSRev1Menu);

	itsCVSRev2Menu->SetMenuItems(kCVSRev2MenuStr);
	itsCVSRev2Menu->SetToPopupChoice(true, itsCVSRev2Cmd);
	itsCVSRev2Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCVSRev2Menu);
	ConfigureCVSRev2Menu(itsCVSRev2Menu);

	itsCVSOnly1StyleMenu->SetStyle(
		JFontStyle(false, false, 0, true, JColorManager::GetRedColor()));
	itsCVSOnly2StyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetBlueColor()));

	ListenTo(itsCVSRev1Input);
	ListenTo(itsCVSRev2Input);

	itsCVSFileInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsCVSFileInput);

	itsCVSChooseButton->SetHint(JGetString("ChoseButtonHint::DiffFileDialog"));

	// SVN

	itsSVNRev1Menu->SetMenuItems(kSVNRev1MenuStr);
	itsSVNRev1Menu->SetToPopupChoice(true, itsSVNRev1Cmd);
	itsSVNRev1Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsSVNRev1Menu);
	ConfigureSVNRev1Menu(itsSVNRev1Menu);

	itsSVNRev2Menu->SetMenuItems(kSVNRev2MenuStr);
	itsSVNRev2Menu->SetToPopupChoice(true, itsSVNRev2Cmd);
	itsSVNRev2Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsSVNRev2Menu);
	ConfigureSVNRev2Menu(itsSVNRev2Menu);

	itsSVNOnly1StyleMenu->SetStyle(
		JFontStyle(false, false, 0, true, JColorManager::GetRedColor()));
	itsSVNOnly2StyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetBlueColor()));

	ListenTo(itsSVNRev1Input);
	ListenTo(itsSVNRev2Input);

	itsSVNFileInput->ShouldAllowInvalidFile(true);
	itsSVNFileInput->ShouldRequireWritable(false);
	itsSVNFileInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsSVNFileInput);

	itsSVNChooseButton->SetHint(JGetString("ChoseButtonHint::DiffFileDialog"));

	// git

	itsGitRev1Menu->SetMenuItems(kGitRev1MenuStr);
	itsGitRev1Menu->SetToPopupChoice(true, itsGitRev1Cmd);
	itsGitRev1Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsGitRev1Menu);
	ConfigureGitRev1Menu(itsGitRev1Menu);

	itsGitRev2Menu->SetMenuItems(kGitRev2MenuStr);
	itsGitRev2Menu->SetToPopupChoice(true, itsGitRev2Cmd);
	itsGitRev2Menu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsGitRev2Menu);
	ConfigureGitRev2Menu(itsGitRev2Menu);

	itsGitOnly1StyleMenu->SetStyle(
		JFontStyle(false, false, 0, true, JColorManager::GetRedColor()));
	itsGitOnly2StyleMenu->SetStyle(
		JFontStyle(false, false, 0, false, JColorManager::GetBlueColor()));

	ListenTo(itsGitRev1Input);
	ListenTo(itsGitRev2Input);

	itsGitFileInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsGitFileInput);

	itsGitChooseButton->SetHint(JGetString("ChoseButtonHint::DiffFileDialog"));

	// misc

	itsStayOpenCB->SetState(true);

	UpdateBasePath();
	ListenTo(GetDocumentManager());

	// create hidden JXDocument so Meta-# shortcuts work

	jnew JXDocumentMenu(JString::empty, window,
					   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);

	HandleGitRevMenu(itsGitRev1Menu, kCurrentRevCmd, &itsGitRev1Cmd, itsGitRev1Input);
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
DiffFileDialog::UpdateDisplay()
{
	JString s;
	if (itsTabIndex == kPlainDiffTabIndex)
	{
		itsViewButton->SetActive(itsPlainFile1Input->GetFile(&s) && itsPlainFile2Input->GetFile(&s));
	}
	else if (itsTabIndex == kCVSDiffTabIndex)
	{
		itsViewButton->SetActive(CheckVCSFileOrPath(itsCVSFileInput, false, nullptr) &&
			(isFixedRevCmd(itsCVSRev1Cmd) || !itsCVSRev1Input->GetText()->IsEmpty()) &&
			(isFixedRevCmd(itsCVSRev2Cmd) || !itsCVSRev2Input->GetText()->IsEmpty()));

		itsCVSRev1Input->SetActive(!isFixedRevCmd(itsCVSRev1Cmd));
		itsCVSRev2Menu->SetActive(itsCVSRev1Cmd != kCurrentRevCmd);
		itsCVSRev2Input->SetActive(!isFixedRevCmd(itsCVSRev2Cmd));
	}
	else if (itsTabIndex == kSVNDiffTabIndex)
	{
		itsViewButton->SetActive(CheckSVNFileOrPath(itsSVNFileInput, false, nullptr) &&
			(isSVNFixedRevCmd(itsSVNRev1Cmd) || !itsSVNRev1Input->GetText()->IsEmpty()) &&
			(isSVNFixedRevCmd(itsSVNRev2Cmd) || !itsSVNRev2Input->GetText()->IsEmpty()));

		itsSVNRev1Input->SetActive(!isSVNFixedRevCmd(itsSVNRev1Cmd));
		itsSVNRev2Menu->SetActive(itsSVNRev1Cmd != kCurrentRevCmd);
		itsSVNRev2Input->SetActive(!isSVNFixedRevCmd(itsSVNRev2Cmd));
	}
	else
	{
		assert( itsTabIndex == kGitDiffTabIndex );

		itsViewButton->SetActive(CheckVCSFileOrPath(itsGitFileInput, false, nullptr) &&
			(isFixedRevCmd(itsGitRev1Cmd) || !itsGitRev1Input->GetText()->IsEmpty()) &&
			(isFixedRevCmd(itsGitRev2Cmd) || !itsGitRev2Input->GetText()->IsEmpty()));

		itsGitRev1Input->SetActive(!isFixedRevCmd(itsGitRev1Cmd));
		itsGitRev2Menu->SetActive(itsGitRev1Cmd != kCurrentRevCmd);
		itsGitRev2Input->SetActive(!isFixedRevCmd(itsGitRev2Cmd));
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
DiffFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsViewButton && message.Is(JXButton::kPushed))
	{
		if (ViewDiffs() && !itsStayOpenCB->IsChecked())
		{
			Deactivate();
		}
	}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
	{
		Deactivate();
	}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("EditorHelp");
	}

	else if (sender == itsPlainChoose1Button && message.Is(JXButton::kPushed))
	{
		ChooseFile(itsPlainFile1Input);
	}
	else if (sender == itsPlainChoose2Button && message.Is(JXButton::kPushed))
	{
		ChooseFile(itsPlainFile2Input);
	}

	else if ((sender == itsPlainFile1Input ||
			  sender == itsPlainFile2Input ||
			  sender == itsCVSFileInput    ||
			  sender == itsCVSRev1Input    ||
			  sender == itsCVSRev2Input    ||
			  sender == itsSVNFileInput    ||
			  sender == itsSVNRev1Input    ||
			  sender == itsSVNRev2Input    ||
			  sender == itsGitFileInput    ||
			  sender == itsGitRev1Input    ||
			  sender == itsGitRev2Input) &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
	{
		UpdateDisplay();
	}

	else if (sender == itsCVSChooseButton && message.Is(JXButton::kPushed))
	{
		if (GetDisplay()->GetLatestKeyModifiers().meta())
		{
			ChoosePath(itsCVSFileInput);
		}
		else
		{
			ChooseFile(itsCVSFileInput);
		}
	}

	else if (sender == itsSVNChooseButton && message.Is(JXButton::kPushed))
	{
		if ((GetDisplay()->GetLatestKeyModifiers()).meta())
		{
			ChoosePath(itsSVNFileInput);
		}
		else
		{
			ChooseFile(itsSVNFileInput);
		}
	}

	else if (sender == itsGitChooseButton && message.Is(JXButton::kPushed))
	{
		if ((GetDisplay()->GetLatestKeyModifiers()).meta())
		{
			ChoosePath(itsGitFileInput);
		}
		else
		{
			ChooseFile(itsGitFileInput);
		}
	}

	else if (sender == itsCVSRev1Menu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateVCSRevMenu(itsCVSRev1Menu, itsCVSRev1Cmd);
	}
	else if (sender == itsCVSRev1Menu && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		HandleCVSRevMenu(itsCVSRev1Menu, selection.GetIndex(), &itsCVSRev1Cmd, itsCVSRev1Input);
	}

	else if (sender == itsCVSRev2Menu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateVCSRevMenu(itsCVSRev2Menu, itsCVSRev2Cmd);
	}
	else if (sender == itsCVSRev2Menu && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		HandleCVSRevMenu(itsCVSRev2Menu, selection.GetIndex(), &itsCVSRev2Cmd, itsCVSRev2Input);
	}

	else if (sender == itsSVNRev1Menu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateVCSRevMenu(itsSVNRev1Menu, itsSVNRev1Cmd);
	}
	else if (sender == itsSVNRev1Menu && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		HandleSVNRevMenu(itsSVNRev1Menu, selection.GetIndex(), &itsSVNRev1Cmd, itsSVNRev1Input);
	}

	else if (sender == itsSVNRev2Menu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateVCSRevMenu(itsSVNRev2Menu, itsSVNRev2Cmd);
	}
	else if (sender == itsSVNRev2Menu && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		HandleSVNRevMenu(itsSVNRev2Menu, selection.GetIndex(), &itsSVNRev2Cmd, itsSVNRev2Input);
	}

	else if (sender == itsGitRev1Menu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateVCSRevMenu(itsGitRev1Menu, itsGitRev1Cmd);
	}
	else if (sender == itsGitRev1Menu && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		HandleGitRevMenu(itsGitRev1Menu, selection.GetIndex(), &itsGitRev1Cmd, itsGitRev1Input);
	}

	else if (sender == itsGitRev2Menu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateVCSRevMenu(itsGitRev2Menu, itsGitRev2Cmd);
	}
	else if (sender == itsGitRev2Menu && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		HandleGitRevMenu(itsGitRev2Menu, selection.GetIndex(), &itsGitRev2Cmd, itsGitRev2Input);
	}

	else if (sender == itsTabGroup->GetCardEnclosure() &&
			 message.Is(JXCardFile::kCardIndexChanged))
	{
		JIndex index;
		if (itsTabGroup->GetCurrentTabIndex(&index) && index != itsTabIndex)
		{
			for (JIndex i=1; i<=kTabCount; i++)
			{
				if (i != itsTabIndex)
				{
					itsStyleMenu[i-1][0]->SetStyle(
						itsStyleMenu[itsTabIndex-1][0]->GetStyle());
					itsStyleMenu[i-1][1]->SetStyle(
						itsStyleMenu[itsTabIndex-1][1]->GetStyle());
				}
			}
			itsTabIndex = index;
		}
		UpdateDisplay();
	}

	else if (sender == GetDocumentManager() &&
			 message.Is(DocumentManager::kProjectDocumentActivated))
	{
		UpdateBasePath();
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ViewDiffs (private)

 ******************************************************************************/

bool
DiffFileDialog::ViewDiffs()
{
	JIndex tabIndex;
	const bool ok = itsTabGroup->GetCurrentTabIndex(&tabIndex);
	assert( ok );

	JString fullName;

	if (tabIndex == kPlainDiffTabIndex &&
		CheckFile(itsPlainFile1Input) &&
		CheckFile(itsPlainFile2Input))
	{
		itsPlainFile1Input->GetText()->DeactivateCurrentUndo();		// undo from latest button press
		itsPlainFile2Input->GetText()->DeactivateCurrentUndo();

		JString file1, file2;
		bool ok = itsPlainFile1Input->GetFile(&file1);
		assert( ok );
		ok = itsPlainFile2Input->GetFile(&file2);
		assert( ok );

		ViewDiffs(file1, file2);
		return true;
	}
	else if (tabIndex == kCVSDiffTabIndex &&
			 CheckVCSFileOrPath(itsCVSFileInput, true, &fullName))
	{
		itsCVSFileInput->GetText()->DeactivateCurrentUndo();			// undo from latest button press
		itsCVSRev1Input->GetText()->DeactivateCurrentUndo();
		itsCVSRev2Input->GetText()->DeactivateCurrentUndo();

		if (JDirectoryExists(fullName))
		{
			DiffDirectory(fullName, "cvs -f diff",
						  itsCVSSummaryCB, " --brief");
		}
		else
		{
			JString getCmd, diffCmd, name1, name2;
			if (BuildCVSDiffCmd(fullName, itsCVSRev1Cmd, &itsCVSRev1Input->GetText()->GetText(),
								itsCVSRev2Cmd, &itsCVSRev2Input->GetText()->GetText(),
								&getCmd, &diffCmd, &name1, &name2, false))
			{
				const JError err =
					DiffDocument::CreateCVS(fullName, getCmd, diffCmd,
											  itsCommonStyleMenu->GetStyle(),
											  name1, itsCVSOnly1StyleMenu->GetStyle(),
											  name2, itsCVSOnly2StyleMenu->GetStyle(),
											  false);
				err.ReportIfError();
			}
		}
		return true;
	}
	else if (tabIndex == kSVNDiffTabIndex &&
			 CheckSVNFileOrPath(itsSVNFileInput, true, &fullName))
	{
		itsSVNFileInput->GetText()->DeactivateCurrentUndo();			// undo from latest button press
		itsSVNRev1Input->GetText()->DeactivateCurrentUndo();
		itsSVNRev2Input->GetText()->DeactivateCurrentUndo();

		if (JDirectoryExists(fullName))
		{
			const JString s = JCombinePathAndName(fullName, kDot) + kDot;
			JString getCmd, diffCmd, name1, name2;
			if (BuildSVNDiffCmd(s, itsSVNRev1Cmd, &itsSVNRev1Input->GetText()->GetText(),
								itsSVNRev2Cmd, &itsSVNRev2Input->GetText()->GetText(),
								&getCmd, &diffCmd, &name1, &name2, false, true))
			{
				DiffDirectory(fullName, diffCmd, itsSVNSummaryCB, " --diff-cmd diff -x --brief");
			}
		}
		else
		{
			JString getCmd, diffCmd, name1, name2;
			if (BuildSVNDiffCmd(fullName,
								itsSVNRev1Cmd, &itsSVNRev1Input->GetText()->GetText(),
								itsSVNRev2Cmd, &itsSVNRev2Input->GetText()->GetText(),
								&getCmd, &diffCmd, &name1, &name2, false))
			{
				const JError err =
					DiffDocument::CreateSVN(fullName, getCmd, diffCmd,
											  itsCommonStyleMenu->GetStyle(),
											  name1, itsSVNOnly1StyleMenu->GetStyle(),
											  name2, itsSVNOnly2StyleMenu->GetStyle(),
											  false);
				err.ReportIfError();
			}
		}
		return true;
	}
	else if (tabIndex == kGitDiffTabIndex &&
			 CheckVCSFileOrPath(itsGitFileInput, true, &fullName))
	{
		itsGitFileInput->GetText()->DeactivateCurrentUndo();			// undo from latest button press
		itsGitRev1Input->GetText()->DeactivateCurrentUndo();
		itsGitRev2Input->GetText()->DeactivateCurrentUndo();

		if (JDirectoryExists(fullName))
		{
			JString diffCmd;
			if (BuildGitDiffDirectoryCmd(fullName,
										 itsGitRev1Cmd, &itsGitRev1Input->GetText()->GetText(),
										 itsGitRev2Cmd, &itsGitRev2Input->GetText()->GetText(),
										 &diffCmd))
			{
				if (itsGitSummaryCB->IsChecked())
				{
					diffCmd += " --summary --stat";
				}
				diffCmd += " .";	// force selected directory
				DiffDirectory(fullName, diffCmd, itsGitSummaryCB, JString::empty);
			}
		}
		else
		{
			JString get1Cmd, get2Cmd, diffCmd, name1, name2;
			if (BuildGitDiffCmd(fullName,
								itsGitRev1Cmd, &itsGitRev1Input->GetText()->GetText(),
								itsGitRev2Cmd, &itsGitRev2Input->GetText()->GetText(),
								&get1Cmd, &get2Cmd, &diffCmd, &name1, &name2, false))
			{
				const JError err =
					DiffDocument::CreateGit(fullName, get1Cmd, get2Cmd, diffCmd,
											itsCommonStyleMenu->GetStyle(),
											name1, itsGitOnly1StyleMenu->GetStyle(),
											name2, itsGitOnly2StyleMenu->GetStyle(),
											false);
				err.ReportIfError();
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 DiffDirectory (private)

 ******************************************************************************/

void
DiffFileDialog::DiffDirectory
	(
	const JString&	fullName,
	const JString&	diffCmd,
	JXCheckbox*		summaryCB,
	const JString&	summaryArgs
	)
{
	const JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
	const JArray<JIndex> lineIndexList;

	auto* path = jnew JString(fullName);

	auto* cmd = jnew JString(diffCmd);

	if (summaryCB->IsChecked())
	{
		*cmd += summaryArgs;
	}

	auto* ss = jnew JString;

	auto* mt = jnew JString;

	auto* ms = jnew JString;

	auto* mi = jnew JString;

	CommandManager::CmdInfo info(path, cmd, ss, false, true, true, true,
								   true, true, false, mt, ms, mi, false);

	CommandManager::Exec(info, nullptr, fullNameList, lineIndexList);

	info.Free();
}

/******************************************************************************
 ViewDiffs

 ******************************************************************************/

void
DiffFileDialog::ViewDiffs
	(
	const JString&	fullName1,
	const JString&	fullName2,
	const bool		silent
	)
{
	if (DocumentManager::WarnFileSize(fullName1) &&
		DocumentManager::WarnFileSize(fullName2))
	{
		itsTabGroup->ShowTab(kPlainDiffTabIndex);		// update styles

		JString cmd = BuildDiffCmd();
		cmd += JPrepArgForExec(fullName1);
		cmd += " ";
		cmd += JPrepArgForExec(fullName2);

		JString path, name1, name2;
		JSplitPathAndName(fullName1, &path, &name1);
		JSplitPathAndName(fullName2, &path, &name2);

		const JError err =
			DiffDocument::CreatePlain(fullName1, cmd, itsCommonStyleMenu->GetStyle(),
										name1, itsPlainOnly1StyleMenu->GetStyle(),
										name2, itsPlainOnly2StyleMenu->GetStyle(),
										silent);
		if (!silent)
		{
			err.ReportIfError();
		}
	}
}

/******************************************************************************
 BuildDiffCmd (private)

	Returns a command to which two file names can be appended.

 ******************************************************************************/

JString
DiffFileDialog::BuildDiffCmd()
{
	JString cmd("diff ");
	if (itsIgnoreSpaceChangeCB->IsChecked())
	{
		cmd += "-b ";		// --ignore-space-change is GNU specific
	}
	if (itsIgnoreBlankLinesCB->IsChecked())
	{
		cmd += "--ignore-blank-lines ";
	}

	return cmd;
}

/******************************************************************************
 ViewVCSDiffs

 ******************************************************************************/

void
DiffFileDialog::ViewVCSDiffs
	(
	const JString&	fullName,
	const bool		silent
	)
{
	if (!DocumentManager::WarnFileSize(fullName))
	{
		return;
	}

	const JVCSType type = JGetVCSType(fullName);
	if (type == kJCVSType)
	{
		ViewCVSDiffs(fullName, silent);
	}
	else if (type == kJSVNType)
	{
		ViewSVNDiffs(fullName, silent);
	}
	else if (type == kJGitType)
	{
		ViewGitDiffs(fullName, silent);
	}
}

/******************************************************************************
 ViewCVSDiffs (private)

 ******************************************************************************/

void
DiffFileDialog::ViewCVSDiffs
	(
	const JString&	fullName,
	const bool		silent
	)
{
	itsTabGroup->ShowTab(kCVSDiffTabIndex);

	JString getCmd, diffCmd, name1, name2;
	if (!BuildCVSDiffCmd(fullName, kCurrentRevCmd, nullptr,
						 kCurrentRevCmd, nullptr,
						 &getCmd, &diffCmd, &name1, &name2, silent))
	{
		return;
	}

	const JError err =
		DiffDocument::CreateCVS(fullName, getCmd, diffCmd,
								  itsCommonStyleMenu->GetStyle(),
								  name1, itsCVSOnly1StyleMenu->GetStyle(),
								  name2, itsCVSOnly2StyleMenu->GetStyle(),
								  silent);
	if (!silent)
	{
		err.ReportIfError();
	}
}

/******************************************************************************
 ViewCVSDiffs

 ******************************************************************************/

void
DiffFileDialog::ViewCVSDiffs
	(
	const JString&	fullName,
	const JString&	rev1,
	const JString&	rev2,
	const bool		silent
	)
{
	if (!DocumentManager::WarnFileSize(fullName))
	{
		return;
	}

	JIndex rev1Cmd = kRevisionNumberCmd, rev2Cmd = kRevisionNumberCmd;
	if (rev1.IsEmpty())
	{
		rev1Cmd = rev2Cmd = kCurrentRevCmd;
	}
	else if (rev2.IsEmpty())
	{
		rev2Cmd = kCurrentRevCmd;
	}

	JString getCmd, diffCmd, name1, name2;
	if (BuildCVSDiffCmd(fullName, rev1Cmd, &rev1, rev2Cmd, &rev2,
						&getCmd, &diffCmd, &name1, &name2, silent))
	{
		const JError err =
			DiffDocument::CreateCVS(fullName, getCmd, diffCmd,
									  itsCommonStyleMenu->GetStyle(),
									  name1, itsCVSOnly1StyleMenu->GetStyle(),
									  name2, itsCVSOnly2StyleMenu->GetStyle(),
									  silent);
		err.ReportIfError();
	}
}

/******************************************************************************
 BuildCVSDiffCmd (private)

 ******************************************************************************/

bool
DiffFileDialog::BuildCVSDiffCmd
	(
	const JString&	fullName,
	const JIndex	rev1Cmd,
	const JString*	origRev1,
	const JIndex	rev2Cmd,
	const JString*	origRev2,
	JString*		getCmd,
	JString*		diffCmd,
	JString*		name1,
	JString*		name2,
	const bool		silent
	)
{
	JString path, name, cvsRoot;
	JSplitPathAndName(fullName, &path, &name);

	cvsRoot = JCombinePathAndName(path, "CVS");
	cvsRoot = JCombinePathAndName(cvsRoot, "Root");
	JReadFile(cvsRoot, &cvsRoot);
	cvsRoot.TrimWhitespace();

	*name1  = name;
	*name1 += ": ";

	*getCmd  = "cvs -d " + JPrepArgForExec(cvsRoot) + " get ";
	*diffCmd = "cvs -f diff ";
	if ((rev1Cmd == kPreviousRevCmd && origRev1 != nullptr) ||
		(!isFixedRevCmd(rev1Cmd) && !JString::IsEmpty(origRev1)))
	{
		JIndex cmd1  = rev1Cmd;
		JString rev1 = *origRev1;
		if (cmd1 == kPreviousRevCmd && GetPreviousCVSRevision(fullName, &rev1))
		{
			cmd1 = kRevisionNumberCmd;
		}
		else if (cmd1 == kPreviousRevCmd)
		{
			if (!silent)
			{
				JGetUserNotification()->ReportError(JGetString("VCSNoPreviousRevision::DiffFileDialog"));
			}
			return false;
		}

		JIndex cmd2  = rev2Cmd;
		JString rev2 = *origRev2;
		if (cmd2 == kPreviousRevCmd && GetCurrentCVSRevision(fullName, &rev2))
		{
			cmd2 = kRevisionNumberCmd;
		}
		else if (cmd2 == kPreviousRevCmd)
		{
			cmd2 = kCurrentRevCmd;
		}

		*getCmd += (cmd1 == kRevisionNumberCmd ? "-r " : "-D ");
		*getCmd += JPrepArgForExec(rev1);
		*getCmd += " ";

		*diffCmd += (cmd1 == kRevisionNumberCmd ? "-r " : "-D ");
		*diffCmd += JPrepArgForExec(rev1);
		*diffCmd += " ";

		*name1 += rev1;

		if (cmd2 != kCurrentRevCmd && !rev2.IsEmpty())
		{
			*diffCmd += (cmd2 == kRevisionNumberCmd ? "-r " : "-D ");
			*diffCmd += JPrepArgForExec(rev2);
			*diffCmd += " ";

			*name2 = rev2;
		}
		else
		{
			*name2 = "edited";
		}
	}
	else
	{
		*name1 += "current";
		*name2  = "edited";

		JString rev;
		if (GetCurrentCVSRevision(fullName, &rev))
		{
			*getCmd += "-r ";
			*getCmd += JPrepArgForExec(rev);
			*getCmd += " ";
		}
	}

	if (itsIgnoreSpaceChangeCB->IsChecked())
	{
		*diffCmd += "-b ";		// --ignore-space-change is GNU specific
	}
	if (itsIgnoreBlankLinesCB->IsChecked())
	{
		*diffCmd += "--ignore-blank-lines ";
	}
	*diffCmd += JPrepArgForExec(name);

	return true;
}

/******************************************************************************
 GetCurrentCVSRevision (private)

 ******************************************************************************/

bool
DiffFileDialog::GetCurrentCVSRevision
	(
	const JString&	fullName,
	JString*		rev
	)
{
	JString path, name, data, pattern;
	JSplitPathAndName(fullName, &path, &name);
	pattern = "^[^/]*/" + JRegex::BackslashForLiteral(name) + "/([0-9.]+)/";
	name    = JCombinePathAndName(path, "CVS");
	name    = JCombinePathAndName(name, "Entries");
	JReadFile(name, &data);

	JRegex r(pattern);
	const JStringMatch m = r.Match(data, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
	{
		*rev = m.GetSubstring(1);
		return true;
	}
	else
	{
		rev->Clear();
		return false;
	}
}

/******************************************************************************
 GetPreviousCVSRevision (private)

 ******************************************************************************/

bool
DiffFileDialog::GetPreviousCVSRevision
	(
	const JString&	fullName,
	JString*		rev
	)
{
	if (!GetCurrentCVSRevision(fullName, rev))
	{
		return false;
	}

	JStringIterator iter(rev, JStringIterator::kStartAtEnd);
	iter.BeginMatch();
	if (!iter.Prev("."))
	{
		return false;
	}

	const JStringMatch& m = iter.FinishMatch();
	if (m.IsEmpty())
	{
		return false;
	}

	JUInt j;
	if (!m.GetString().ConvertToUInt(&j))
	{
		return false;
	}
	else if (j > 1)
	{
		iter.RemoveAllNext();

		*rev += JString(j-1);
		return true;
	}

	iter.BeginMatch();
	if (!iter.Prev("."))
	{
		return false;
	}

	iter.RemoveAllNext();
	return true;
}

/******************************************************************************
 ViewSVNDiffs (private)

 ******************************************************************************/

void
DiffFileDialog::ViewSVNDiffs
	(
	const JString&	fullName,
	const bool		silent
	)
{
	itsTabGroup->ShowTab(kSVNDiffTabIndex);

	JString getCmd, diffCmd, name1, name2;
	if (!BuildSVNDiffCmd(fullName, kCurrentRevCmd, nullptr, kCurrentRevCmd, nullptr,
						 &getCmd, &diffCmd, &name1, &name2, silent))
	{
		return;
	}

	const JError err =
		DiffDocument::CreateSVN(fullName, getCmd, diffCmd,
								  itsCommonStyleMenu->GetStyle(),
								  name1, itsSVNOnly1StyleMenu->GetStyle(),
								  name2, itsSVNOnly2StyleMenu->GetStyle(),
								  silent);
	if (!silent)
	{
		err.ReportIfError();
	}
}

/******************************************************************************
 ViewSVNDiffs

 ******************************************************************************/

void
DiffFileDialog::ViewSVNDiffs
	(
	const JString&	fullName,
	const JString&	rev1,
	const JString&	rev2,
	const bool		silent
	)
{
	if (!DocumentManager::WarnFileSize(fullName))
	{
		return;
	}

	JIndex rev1Cmd = kRevisionNumberCmd, rev2Cmd = kRevisionNumberCmd;
	if (rev1.IsEmpty())
	{
		rev1Cmd = rev2Cmd = kCurrentRevCmd;
	}
	else if (rev2.IsEmpty())
	{
		rev2Cmd = kCurrentRevCmd;
	}

	JString getCmd, diffCmd, name1, name2;
	if (BuildSVNDiffCmd(fullName, rev1Cmd, &rev1, rev2Cmd, &rev2,
						&getCmd, &diffCmd, &name1, &name2, silent))
	{
		const JError err =
			DiffDocument::CreateSVN(fullName, getCmd, diffCmd,
									  itsCommonStyleMenu->GetStyle(),
									  name1, itsSVNOnly1StyleMenu->GetStyle(),
									  name2, itsSVNOnly2StyleMenu->GetStyle(),
									  silent);
		err.ReportIfError();
	}
}

/******************************************************************************
 BuildSVNDiffCmd (private)

 ******************************************************************************/

bool
DiffFileDialog::BuildSVNDiffCmd
	(
	const JString&	fullName,
	const JIndex	rev1Cmd,
	const JString*	origRev1,
	const JIndex	rev2Cmd,
	const JString*	origRev2,
	JString*		getCmd,
	JString*		diffCmd,
	JString*		name1,
	JString*		name2,
	const bool		silent,
	const bool		forDirectory
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	*name1  = name;
	*name1 += ": ";

	JString file1 = fullName;
	JString file2 = fullName;

	*getCmd  = "svn cat ";
	*diffCmd = (forDirectory ? "svn diff " : "svn diff --diff-cmd diff -x --normal ");
	if ((rev1Cmd == kPreviousRevCmd && origRev1 != nullptr) ||		// PREV from dialog
		(rev1Cmd == kTrunkCmd && origRev1 != nullptr) ||			// TRUNK from dialog
		(!isSVNFixedRevCmd(rev1Cmd) && !JString::IsEmpty(origRev1)))
	{
		JIndex cmd1    = rev1Cmd;
		JString rev1   = *origRev1;
		JString getRev = *origRev1;
		JString revName;
		if (cmd1 == kPreviousRevCmd)
		{
			cmd1    = kRevisionNumberCmd;
			rev1    = "PREV";
			getRev  = "PREV";
			*name1 += rev1;
		}
		else if (cmd1 == kRevisionDateCmd)
		{
			getRev = rev1 = "{" + rev1 + "}";
			*name1 += rev1;
		}
		else if (cmd1 == kTrunkCmd  ||
				 cmd1 == kBranchCmd ||
				 cmd1 == kTagCmd)
		{
			if (!BuildSVNRepositoryPath(&file1, cmd1, rev1, &revName, silent))
			{
				return false;
			}
			getRev = rev1 = "HEAD";
			*name1 += revName;
		}
		else
		{
			*name1 += rev1;
		}

		JIndex cmd2  = rev2Cmd;
		JString rev2 = *origRev2;
		if (cmd2 == kPreviousRevCmd)
		{
			cmd2   = kRevisionNumberCmd;
			rev2   = "COMMITTED";
			*name2 = rev2;
		}
		else if (cmd2 == kRevisionNumberCmd)
		{
			*name2 = rev2;
		}
		else if (cmd2 == kRevisionDateCmd)
		{
			rev2   = "{" + rev2 + "}";
			*name2 = rev2;
		}
		else if (cmd2 == kTrunkCmd  ||
				 cmd2 == kBranchCmd ||
				 cmd2 == kTagCmd)
		{
			if (!BuildSVNRepositoryPath(&file2, cmd2, rev2, &revName, silent))
			{
				return false;
			}
			rev2   = "HEAD";
			*name2 = revName;
		}
		else if (JIsURL(fullName))
		{
			*name2 = "current";
		}
		else
		{
			*name2 = "edited";
		}

		*getCmd += "-r ";
		*getCmd += JPrepArgForExec(getRev);
		*getCmd += " ";
		*getCmd += JPrepArgForExec(file1);

		*diffCmd += "-r ";
		*diffCmd += JPrepArgForExec(rev1);

		if (cmd2 != kCurrentRevCmd && !rev2.IsEmpty())
		{
			*diffCmd += ":";
			*diffCmd += JPrepArgForExec(rev2);
		}
	}
	else
	{
		*name1  += "current";
		*name2   = "edited";
		*getCmd += "-r BASE ";
		*getCmd += JPrepArgForExec(file1);
	}

	if (itsIgnoreSpaceChangeCB->IsChecked())
	{
		*diffCmd += " -x -b ";		// --ignore-space-change is GNU specific
	}
	if (itsIgnoreBlankLinesCB->IsChecked())
	{
		*diffCmd += " -x --ignore-blank-lines ";
	}

	if (!forDirectory && file1 == file2)
	{
		*diffCmd += " ";
		*diffCmd += JPrepArgForExec(file1);
	}
	else
	{
		*diffCmd += " --old=";
		*diffCmd += JPrepArgForExec(file1);
		*diffCmd += " --new=";
		*diffCmd += JPrepArgForExec(file2);
	}

	return true;
}

/******************************************************************************
 BuildSVNRepositoryPath (private)

 ******************************************************************************/

static const JRegex theBranchPattern("(?<=/)(trunk($|(?=/))|(branches|tags)/[^/]+(?=/))");

bool
DiffFileDialog::BuildSVNRepositoryPath
	(
	JString*		fullName,
	const JIndex	cmd,
	const JString&	rev,
	JString*		name,
	const bool		silent
	)
{
	JString repoPath;
	if (!JGetVCSRepositoryPath(*fullName, &repoPath))
	{
		if (!silent)
		{
			JGetUserNotification()->ReportError(JGetString("SVNNoRepository::DiffFileDialog"));
		}
		return false;
	}

	JStringIterator iter(&repoPath);
	if (!iter.Next(theBranchPattern))
	{
		if (!silent)
		{
			JGetUserNotification()->ReportError(JGetString("SVNNonstandardRepository::DiffFileDialog"));
		}
		return false;
	}

	name->Clear();
	if (cmd == kTrunkCmd)
	{
		*name = "trunk";
	}
	else if (cmd == kBranchCmd)
	{
		*name  = "branches/";
		*name += rev;
	}
	else if (cmd == kTagCmd)
	{
		*name  = "tags/";
		*name += rev;
	}
	assert( !name->IsEmpty() );

	iter.ReplaceLastMatch(*name);
	*fullName = repoPath;
	return true;
}

/******************************************************************************
 ViewGitDiffs (private)

 ******************************************************************************/

void
DiffFileDialog::ViewGitDiffs
	(
	const JString&	fullName,
	const bool		silent
	)
{
	itsTabGroup->ShowTab(kGitDiffTabIndex);

	JString get1Cmd, get2Cmd, diffCmd, name1, name2;
	if (!BuildGitDiffCmd(fullName, kCurrentRevCmd, nullptr, kCurrentRevCmd, nullptr,
						 &get1Cmd, &get2Cmd, &diffCmd, &name1, &name2, silent))
	{
		return;
	}

	const JError err =
		DiffDocument::CreateGit(fullName, get1Cmd, get2Cmd, diffCmd,
								  itsCommonStyleMenu->GetStyle(),
								  name1, itsGitOnly1StyleMenu->GetStyle(),
								  name2, itsGitOnly2StyleMenu->GetStyle(),
								  silent);
	if (!silent)
	{
		err.ReportIfError();
	}
}

/******************************************************************************
 ViewGitDiffs

 ******************************************************************************/

void
DiffFileDialog::ViewGitDiffs
	(
	const JString&	fullName,
	const JString&	rev1,
	const JString&	rev2,
	const bool		silent
	)
{
	if (!DocumentManager::WarnFileSize(fullName))
	{
		return;
	}

	JIndex rev1Cmd = kRevisionNumberCmd, rev2Cmd = kRevisionNumberCmd;
	if (rev1.IsEmpty())
	{
		rev1Cmd = kCurrentRevCmd;
		rev2Cmd = kCurrentRevCmd;
	}
	else if (rev2.IsEmpty())
	{
		rev2Cmd = kCurrentRevCmd;
	}

	JString get1Cmd, get2Cmd, diffCmd, name1, name2;
	if (BuildGitDiffCmd(fullName, rev1Cmd, &rev1, rev2Cmd, &rev2,
						&get1Cmd, &get2Cmd, &diffCmd, &name1, &name2, silent))
	{
		const JError err =
			DiffDocument::CreateGit(fullName, get1Cmd, get2Cmd, diffCmd,
									  itsCommonStyleMenu->GetStyle(),
									  name1, itsGitOnly1StyleMenu->GetStyle(),
									  name2, itsGitOnly2StyleMenu->GetStyle(),
									  silent);
		err.ReportIfError();
	}
}

/******************************************************************************
 BuildGitDiffCmd (private)

 ******************************************************************************/

bool
DiffFileDialog::BuildGitDiffCmd
	(
	const JString&	fullName,
	const JIndex	rev1Cmd,
	const JString*	rev1,
	const JIndex	rev2Cmd,
	const JString*	rev2,
	JString*		get1Cmd,
	JString*		get2Cmd,
	JString*		diffCmd,
	JString*		name1,
	JString*		name2,
	const bool		silent
	)
{
	JString trueName;
	if (!JGetTrueName(fullName, &trueName))
	{
		return false;
	}

	JString path, name;
	JSplitPathAndName(trueName, &path, &name);

	// git show must be run with path relative to repo root

	JString gitRoot;
	if (!JSearchGitRoot(path, &gitRoot))
	{
		return false;
	}

	JString gitFile = JConvertToRelativePath(trueName, gitRoot);
	if (gitFile.StartsWith("." ACE_DIRECTORY_SEPARATOR_STR))
	{
		JStringIterator iter(&gitFile);
		iter.RemoveNext(2);		// ACE_DIRECTORY_SEPARATOR_CHAR guarantees length 1
	}

	*name1  = name;
	*name1 += ": ";

	get1Cmd->Clear();
	get2Cmd->Clear();
	if ((rev1Cmd == kPreviousRevCmd && rev1 != nullptr) ||		// PREV from dialog
		(!isFixedRevCmd(rev1Cmd) && !JString::IsEmpty(rev1)))
	{
		JString get1Rev;
		if (rev1Cmd == kPreviousRevCmd)
		{
			if (!GetPreviousGitRevision(trueName, &get1Rev))
			{
				if (!silent)
				{
					JGetUserNotification()->ReportError(JGetString("VCSNoPreviousRevision::DiffFileDialog"));
				}
				return false;
			}

			*name1 += "previous";
		}
		else if (rev1Cmd == kRevisionDateCmd)
		{
			get1Rev  = "@{";
			get1Rev += *rev1;
			get1Rev += "}";

			*name1 += get1Rev;
		}
		else if (rev1Cmd != kGit1AncestorCmd)
		{
			get1Rev = *rev1;
			*name1 += *rev1;
		}

		JString get2Rev;
		if (rev2Cmd == kCurrentRevCmd)
		{
			*get2Cmd  = "file:";
			*get2Cmd += trueName;

			*name2 = "edited";
		}
		else if (rev2Cmd == kPreviousRevCmd)
		{
			*name2 = "current";
		}
		else if (rev2Cmd == kRevisionDateCmd)
		{
			get2Rev  = "@{";
			get2Rev += *rev2;
			get2Rev += "}";

			*name2 = get2Rev;
		}
		else
		{
			get2Rev = *rev2;
			*name2  = *rev2;
		}

		if (rev1Cmd == kGit1AncestorCmd)
		{
			get1Rev = *rev1;
			if (!GetBestCommonGitAncestor(path, &get1Rev, get2Rev.GetBytes()))
			{
				if (!silent)
				{
					JGetUserNotification()->ReportError(JGetString("GitNoCommonAncestor::DiffFileDialog"));
				}
				return false;
			}

			*name1 += get1Rev;
		}

		const JString n = JPrepArgForExec(gitFile);

		*get1Cmd  = "git show ";
		*get1Cmd += JPrepArgForExec(get1Rev);
		*get1Cmd += ":";
		*get1Cmd += n;

		if (get2Cmd->IsEmpty())
		{
			*get2Cmd  = "git show ";
			*get2Cmd += JPrepArgForExec(get2Rev);
			*get2Cmd += ":";
			*get2Cmd += n;
		}
	}
	else
	{
		*name1 += "current";
		*name2  = "edited";

		*get1Cmd  = "git show :";
		*get1Cmd += JPrepArgForExec(gitFile);

		*get2Cmd  = "file:";
		*get2Cmd += trueName;
	}

	*diffCmd = BuildDiffCmd();
	return true;
}

/******************************************************************************
 BuildGitDiffDirectoryCmd (private)

 ******************************************************************************/

bool
DiffFileDialog::BuildGitDiffDirectoryCmd
	(
	const JString&	path,
	const JIndex	rev1Cmd,
	const JString*	rev1,
	const JIndex	rev2Cmd,
	const JString*	rev2,
	JString*		diffCmd
	)
{
	JString trueName;
	if (!JGetTrueName(path, &trueName))
	{
		return false;
	}

	// git show must be run with path relative to repo root

	JString gitRoot;
	if (!JSearchGitRoot(path, &gitRoot))
	{
		return false;
	}

	*diffCmd = "git diff ";
	if (itsIgnoreSpaceChangeCB->IsChecked())
	{
		*diffCmd += "--ignore-space-change ";
	}

	if ((rev1Cmd == kPreviousRevCmd && rev1 != nullptr) ||		// PREV from dialog
		(!isFixedRevCmd(rev1Cmd) && !JString::IsEmpty(rev1)))
	{
		JString get1Rev;
		if (rev1Cmd == kPreviousRevCmd)
		{
			const JString s = JCombinePathAndName(trueName, kDot) + kDot;
			if (!GetPreviousGitRevision(s, &get1Rev))
			{
				JGetUserNotification()->ReportError(JGetString("VCSNoPreviousRevision::DiffFileDialog"));
				return false;
			}
		}
		else if (rev1Cmd == kRevisionDateCmd)
		{
			get1Rev  = "@{";
			get1Rev += *rev1;
			get1Rev += "}";
		}
		else
		{
			get1Rev = *rev1;
		}

		*diffCmd += JPrepArgForExec(get1Rev);
		*diffCmd += (rev1Cmd == kGit1AncestorCmd ? "..." : " ");

		JString get2Rev;
		if (rev2Cmd == kCurrentRevCmd)
		{
			// leave blank
		}
		else if (rev2Cmd == kPreviousRevCmd)
		{
			const JString s = JCombinePathAndName(trueName, kDot) + kDot;
			if (!GetCurrentGitRevision(s, &get1Rev))
			{
				JGetUserNotification()->ReportError(JGetString("VCSNoCurrentRevision::DiffFileDialog"));
				return false;
			}
		}
		else if (rev2Cmd == kRevisionDateCmd)
		{
			get2Rev  = "@{";
			get2Rev += *rev2;
			get2Rev += "}";
		}
		else
		{
			get2Rev = *rev2;
		}

		*diffCmd += JPrepArgForExec(get2Rev);
	}

	return true;
}

/******************************************************************************
 GetCurrentGitRevision (private)

 ******************************************************************************/

bool
DiffFileDialog::GetCurrentGitRevision
	(
	const JString&	fullName,
	JString*		rev
	)
{
	JString s;
	return GetLatestGitRevisions(fullName, rev, &s);
}

/******************************************************************************
 GetPreviousGitRevision (private)

 ******************************************************************************/

bool
DiffFileDialog::GetPreviousGitRevision
	(
	const JString&	fullName,
	JString*		rev
	)
{
	JString s;
	return GetLatestGitRevisions(fullName, &s, rev);
}

/******************************************************************************
 GetLatestGitRevisions (private)

 ******************************************************************************/

bool
DiffFileDialog::GetLatestGitRevisions
	(
	const JString&	fullName,
	JString*		rev1,
	JString*		rev2
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JUtf8Byte* args[] =
	{ "git", "log", "-2", "--format=oneline", name.GetBytes(), nullptr };

	JProcess* p;
	int fromFD;
	const JError err = JProcess::Create(&p, path, args, sizeof(args),
										kJIgnoreConnection, nullptr,
										kJCreatePipe, &fromFD);
	if (err.OK())
	{
		*rev1 = JReadUntil(fromFD, ' ');	// current commit
		JIgnoreUntil(fromFD, '\n');			// ignore comment
		*rev2 = JReadUntil(fromFD, ' ');	// previous commit
		close(fromFD);
		return true;
	}
	else
	{
		err.ReportIfError();
		rev1->Clear();
		rev2->Clear();
		return false;
	}
}

/******************************************************************************
 GetBestCommonGitAncestor (private)

 ******************************************************************************/

bool
DiffFileDialog::GetBestCommonGitAncestor
	(
	const JString&		path,
	JString*			rev1,
	const JUtf8Byte*	rev2
	)
{
	if (JString::IsEmpty(rev2))
	{
		rev2 = "HEAD";
	}

	const JUtf8Byte* args[] =
	{ "git", "merge-base", rev1->GetBytes(), rev2, nullptr };

	JProcess* p;
	int fromFD;
	const JError err = JProcess::Create(&p, path, args, sizeof(args),
										kJIgnoreConnection, nullptr,
										kJCreatePipe, &fromFD);
	if (err.OK())
	{
		if (!JReadAll(fromFD, rev1))
		{
			return false;
		}
		close(fromFD);
		rev1->TrimWhitespace();
		return true;
	}
	else
	{
		err.ReportIfError();
		rev1->Clear();
		return false;
	}
}

/******************************************************************************
 GetSmartDiffItemText

 ******************************************************************************/

const JString&
DiffFileDialog::GetSmartDiffItemText
	(
	const bool		onDisk,
	const JString&	fullName,
	bool*			enable
	)
	const
{
	if (!onDisk)
	{
		*enable = false;
		return JGetString("DiffBackupItemText::DiffFileDialog");
	}

	JString path, origFileName;
	JSplitPathAndName(fullName, &path, &origFileName);

	bool isSafetySave, isBackup;
	JString fileName = GetSmartDiffInfo(origFileName, &isSafetySave, &isBackup);

	const JUtf8Byte* id = nullptr;
	if (isSafetySave)
	{
		const JString savedName = JCombinePathAndName(path, fileName);
		*enable = JFileReadable(savedName);
		id      = "DiffSavedItemText::DiffFileDialog";
	}
	else if (isBackup)
	{
		const JString newName = JCombinePathAndName(path, fileName);
		*enable = JFileReadable(newName);
		id      = "DiffNewItemText::DiffFileDialog";
	}
	else
	{
		JString backupName = fullName;
		backupName.Append("~");
		*enable = JFileReadable(backupName);
		id      = "DiffBackupItemText::DiffFileDialog";
	}
	assert( id != nullptr );
	return JGetString(id);
}

/******************************************************************************
 ViewDiffs

	We require onDisk to avoid being passed an invalid fullName.

 ******************************************************************************/

void
DiffFileDialog::ViewDiffs
	(
	const bool		onDisk,
	const JString&	fullName,
	const bool		silent
	)
{
	if (!onDisk)
	{
		return;
	}

	JString path, origFileName;
	JSplitPathAndName(fullName, &path, &origFileName);

	bool isSafetySave, isBackup;
	JString fileName = GetSmartDiffInfo(origFileName, &isSafetySave, &isBackup);

	if (isSafetySave)
	{
		const JString savedName = JCombinePathAndName(path, fileName);
		ViewDiffs(savedName, fullName, silent);
	}
	else if (isBackup)
	{
		const JString newName = JCombinePathAndName(path, fileName);
		ViewDiffs(fullName, newName, silent);
	}
	else
	{
		JString backupName = fullName;
		backupName.Append("~");
		if (JFileExists(backupName))
		{
			ViewDiffs(backupName, fullName, silent);
		}
	}
}

/******************************************************************************
 GetSmartDiffInfo (private)

	Returns the name of the file without # or ~

 ******************************************************************************/

JString
DiffFileDialog::GetSmartDiffInfo
	(
	const JString&	origFileName,
	bool*			isSafetySave,
	bool*			isBackup
	)
	const
{
	JString fileName = origFileName;

	JStringIterator iter(&fileName);

	*isSafetySave = false;
	while (fileName.StartsWith("#") && fileName.EndsWith("#"))
	{
		iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
		iter.RemoveNext();

		iter.MoveTo(JStringIterator::kStartAtEnd, 0);
		iter.RemovePrev();

		*isSafetySave = true;
	}

	iter.MoveTo(JStringIterator::kStartAtEnd, 0);

	*isBackup = false;
	while (!(*isSafetySave) && fileName.EndsWith("~"))		// don't strip if safety save
	{
		iter.RemovePrev();
		*isBackup = true;
	}

	return fileName;
}

/******************************************************************************
 CheckFile (private)

 ******************************************************************************/

bool
DiffFileDialog::CheckFile
	(
	JXFileInput* widget
	)
	const
{
	widget->ShouldAllowInvalidFile(false);
	const bool valid = widget->InputValid();
	widget->ShouldAllowInvalidFile(true);
	if (!valid)
	{
		widget->Focus();
	}
	return valid;
}

/******************************************************************************
 ChooseFile (private)

 ******************************************************************************/

void
DiffFileDialog::ChooseFile
	(
	JXFileInput* widget
	)
{
	auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile, widget->GetTextForChooseFile());
	if (dlog->DoDialog())
	{
		widget->Focus();
		widget->GetText()->SetText(dlog->GetFullName());
	}
}

/******************************************************************************
 ChoosePath (private)

 ******************************************************************************/

void
DiffFileDialog::ChoosePath
	(
	JXFileInput* widget
	)
{
	auto* dlog = JXChoosePathDialog::Create(JXChoosePathDialog::kAcceptReadable, widget->GetTextForChooseFile());
	if (dlog->DoDialog())
	{
		widget->Focus();
		widget->GetText()->SetText(dlog->GetPath());
	}
}

/******************************************************************************
 CheckVCSFileOrPath (private)

 ******************************************************************************/

bool
DiffFileDialog::CheckVCSFileOrPath
	(
	JXFileInput*	widget,
	const bool		reportError,
	JString*		fullName		// can be nullptr
	)
	const
{
	const JString& text = widget->GetText()->GetText();
	if (!text.IsEmpty())
	{
		JString basePath, s;
		widget->GetBasePath(&basePath);
		if (JConvertToAbsolutePath(text, basePath, &s))
		{
			if (fullName != nullptr)
			{
				*fullName = s;
			}
			return true;
		}
	}

	if (reportError)
	{
		widget->Focus();
		JGetUserNotification()->ReportError(JGetString("VCSDiffTargetInvalid::DiffFileDialog"));
	}

	if (fullName != nullptr)
	{
		fullName->Clear();
	}
	return false;
}

/******************************************************************************
 UpdateVCSRevMenu (private)

 ******************************************************************************/

void
DiffFileDialog::UpdateVCSRevMenu
	(
	JXTextMenu*		menu,
	const JIndex	cmd
	)
{
	menu->CheckItem(cmd);
}

/******************************************************************************
 HandleCVSRevMenu (private)

 ******************************************************************************/

void
DiffFileDialog::HandleCVSRevMenu
	(
	JXTextMenu*		menu,
	const JIndex	index,
	JIndex*			cmd,
	JXInputField*	input
	)
{
	*cmd = index;

	if (menu == itsCVSRev1Menu && index == kCurrentRevCmd)
	{
		itsCVSRev2Cmd = kCurrentRevCmd;
		itsCVSRev2Menu->SetPopupChoice(itsCVSRev2Cmd);
	}

	UpdateDisplay();
	input->Focus();
}

/******************************************************************************
 CheckSVNFileOrPath (private)

 ******************************************************************************/

bool
DiffFileDialog::CheckSVNFileOrPath
	(
	JXFileInput*	widget,
	const bool		reportError,
	JString*		fullName		// can be nullptr
	)
	const
{
	const JString& text = widget->GetText()->GetText();
	if (!text.IsEmpty())
	{
		if (JIsURL(text))
		{
			if (fullName != nullptr)
			{
				*fullName = text;
			}
			return true;
		}

		JString basePath, s;
		widget->GetBasePath(&basePath);
		if (JConvertToAbsolutePath(text, basePath, &s))
		{
			if (fullName != nullptr)
			{
				*fullName = s;
			}
			return true;
		}
	}

	if (reportError)
	{
		widget->Focus();
		JGetUserNotification()->ReportError(JGetString("SVNDiffTargetInvalid::DiffFileDialog"));
	}

	if (fullName != nullptr)
	{
		fullName->Clear();
	}
	return false;
}

/******************************************************************************
 HandleSVNRevMenu (private)

 ******************************************************************************/

void
DiffFileDialog::HandleSVNRevMenu
	(
	JXTextMenu*		menu,
	const JIndex	index,
	JIndex*			cmd,
	JXInputField*	input
	)
{
	*cmd = index;

	if (menu == itsSVNRev1Menu && index == kCurrentRevCmd)
	{
		itsSVNRev2Cmd = kCurrentRevCmd;
		itsSVNRev2Menu->SetPopupChoice(itsSVNRev2Cmd);
	}

	UpdateDisplay();
	input->Focus();
}

/******************************************************************************
 HandleGitRevMenu (private)

 ******************************************************************************/

void
DiffFileDialog::HandleGitRevMenu
	(
	JXTextMenu*		menu,
	const JIndex	index,
	JIndex*			cmd,
	JXInputField*	input
	)
{
	*cmd = index;

	if (menu == itsGitRev1Menu && index == kCurrentRevCmd)
	{
		itsGitRev2Cmd = kCurrentRevCmd;
		itsGitRev2Menu->SetPopupChoice(itsGitRev2Cmd);
	}

	UpdateDisplay();
	input->Focus();
}

/******************************************************************************
 UpdateBasePath (private)

 ******************************************************************************/

void
DiffFileDialog::UpdateBasePath()
{
	ProjectDocument* doc;
	if (GetDocumentManager()->GetActiveProjectDocument(&doc))
	{
		const JString& path = doc->GetFilePath();
		itsPlainFile1Input->SetBasePath(path);
		itsPlainFile2Input->SetBasePath(path);
		itsCVSFileInput->SetBasePath(path);
		itsSVNFileInput->SetBasePath(path);
	}
	else
	{
		itsPlainFile1Input->ClearBasePath();
		itsPlainFile2Input->ClearBasePath();
		itsCVSFileInput->ClearBasePath();
		itsSVNFileInput->ClearBasePath();
	}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
DiffFileDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
	{
		return;
	}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsCommonStyleMenu->ReadStyle(input);

	itsPlainOnly1StyleMenu->ReadStyle(input);
	itsPlainOnly2StyleMenu->ReadStyle(input);

	itsCVSOnly1StyleMenu->SetStyle(itsPlainOnly1StyleMenu->GetStyle());
	itsCVSOnly2StyleMenu->SetStyle(itsPlainOnly2StyleMenu->GetStyle());

	itsSVNOnly1StyleMenu->SetStyle(itsPlainOnly1StyleMenu->GetStyle());
	itsSVNOnly2StyleMenu->SetStyle(itsPlainOnly2StyleMenu->GetStyle());

	itsGitOnly1StyleMenu->SetStyle(itsPlainOnly1StyleMenu->GetStyle());
	itsGitOnly2StyleMenu->SetStyle(itsPlainOnly2StyleMenu->GetStyle());

	if (vers >= 2)
	{
		bool ignoreSpaceChange, ignoreBlankLines;
		input >> JBoolFromString(ignoreSpaceChange)
			  >> JBoolFromString(ignoreBlankLines);
		itsIgnoreSpaceChangeCB->SetState(ignoreSpaceChange);
		itsIgnoreBlankLinesCB->SetState(ignoreBlankLines);
	}

	if (vers >= 1)
	{
		bool stayOpen;
		input >> JBoolFromString(stayOpen);
		itsStayOpenCB->SetState(stayOpen);
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
DiffFileDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsCommonStyleMenu->WriteStyle(output);

	if (itsTabIndex == kPlainDiffTabIndex)
	{
		output << ' ';
		itsPlainOnly1StyleMenu->WriteStyle(output);
		output << ' ';
		itsPlainOnly2StyleMenu->WriteStyle(output);
	}
	else if (itsTabIndex == kCVSDiffTabIndex)
	{
		output << ' ';
		itsCVSOnly1StyleMenu->WriteStyle(output);
		output << ' ';
		itsCVSOnly2StyleMenu->WriteStyle(output);
	}
	else if (itsTabIndex == kSVNDiffTabIndex)
	{
		output << ' ';
		itsSVNOnly1StyleMenu->WriteStyle(output);
		output << ' ';
		itsSVNOnly2StyleMenu->WriteStyle(output);
	}
	else
	{
		assert( itsTabIndex == kGitDiffTabIndex );

		output << ' ';
		itsGitOnly1StyleMenu->WriteStyle(output);
		output << ' ';
		itsGitOnly2StyleMenu->WriteStyle(output);
	}

	output << ' ' << JBoolToString(itsIgnoreSpaceChangeCB->IsChecked())
				  << JBoolToString(itsIgnoreBlankLinesCB->IsChecked())
				  << JBoolToString(itsStayOpenCB->IsChecked());
	output << ' ';
}
