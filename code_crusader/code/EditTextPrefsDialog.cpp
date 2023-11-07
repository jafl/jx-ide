/******************************************************************************
 EditTextPrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "EditTextPrefsDialog.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "SampleText.h"
#include "SearchTextDialog.h"
#include "FnMenuUpdater.h"
#include "Emulator.h"
#include "sharedUtil.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXCharInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXChooseMonoFont.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXChooseColorDialog.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

// emulators

static const JUtf8Byte* kEmulatorMenuStr =
	"  None %r"
	"| vi   %r";

enum
{
	kNoEmulatorCmd = 1,
	kVIEmulatorCmd
};

static const Emulator kMenuIndexToEmulator[] =
{
	kNoEmulator,
	kVIEmulator
};

static const JIndex kEmulatorToMenuIndex[] =
{
	kNoEmulatorCmd,
	kVIEmulatorCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

EditTextPrefsDialog::EditTextPrefsDialog
	(
	TextDocument* doc
	)
	:
	JXModalDialogDirector(),
	itsDoc(doc)
{
	itsOrigEmulatorIndex =
		itsEmulatorIndex = kEmulatorToMenuIndex[
			GetPrefsManager()->GetEmulator() ];

	BuildWindow(doc);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditTextPrefsDialog::~EditTextPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
EditTextPrefsDialog::BuildWindow
	(
	TextDocument* doc
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 640,530, JString::empty);

	itsTabCharCountInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 540,130, 40,20);
	assert( itsTabCharCountInput != nullptr );

	itsCRMLineWidthInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 540,150, 40,20);
	assert( itsCRMLineWidthInput != nullptr );

	itsUndoDepthInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 540,170, 40,20);
	assert( itsUndoDepthInput != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,490, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,490, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::EditTextPrefsDialog::shortcuts::JXLayout"));

	itsCreateBackupCB =
		jnew JXTextCheckbox(JGetString("itsCreateBackupCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,230, 300,20);
	assert( itsCreateBackupCB != nullptr );

	itsAutoIndentCB =
		jnew JXTextCheckbox(JGetString("itsAutoIndentCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,160, 280,20);
	assert( itsAutoIndentCB != nullptr );

	auto* spacesPerTabLabel =
		jnew JXStaticText(JGetString("spacesPerTabLabel::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,130, 210,20);
	assert( spacesPerTabLabel != nullptr );
	spacesPerTabLabel->SetToLabel();

	itsExtraSpaceWindTitleCB =
		jnew JXTextCheckbox(JGetString("itsExtraSpaceWindTitleCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,310, 300,20);
	assert( itsExtraSpaceWindTitleCB != nullptr );

	itsOpenComplFileOnTopCB =
		jnew JXTextCheckbox(JGetString("itsOpenComplFileOnTopCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,280, 300,20);
	assert( itsOpenComplFileOnTopCB != nullptr );

	itsFontMenu =
		jnew JXChooseMonoFont(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 280,60);
	assert( itsFontMenu != nullptr );

	itsEmulatorMenu =
		jnew JXTextMenu(JGetString("itsEmulatorMenu::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 280,25);
	assert( itsEmulatorMenu != nullptr );

	auto* undoDepthLabel =
		jnew JXStaticText(JGetString("undoDepthLabel::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,170, 210,20);
	assert( undoDepthLabel != nullptr );
	undoDepthLabel->SetToLabel();

	itsUseDNDCB =
		jnew JXTextCheckbox(JGetString("itsUseDNDCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,130, 280,20);
	assert( itsUseDNDCB != nullptr );

	itsOnlyBackupIfNoneCB =
		jnew JXTextCheckbox(JGetString("itsOnlyBackupIfNoneCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,250, 300,20);
	assert( itsOnlyBackupIfNoneCB != nullptr );

	itsLeftToFrontOfTextCB =
		jnew JXTextCheckbox(JGetString("itsLeftToFrontOfTextCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,450, 300,20);
	assert( itsLeftToFrontOfTextCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,490, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::EditTextPrefsDialog::shortcuts::JXLayout"));

	itsBalanceWhileTypingCB =
		jnew JXTextCheckbox(JGetString("itsBalanceWhileTypingCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,310, 280,20);
	assert( itsBalanceWhileTypingCB != nullptr );

	itsScrollToBalanceCB =
		jnew JXTextCheckbox(JGetString("itsScrollToBalanceCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,330, 280,20);
	assert( itsScrollToBalanceCB != nullptr );

	itsBeepWhenTypeUnbalancedCB =
		jnew JXTextCheckbox(JGetString("itsBeepWhenTypeUnbalancedCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,350, 280,20);
	assert( itsBeepWhenTypeUnbalancedCB != nullptr );

	auto* cpmLabel =
		jnew JXStaticText(JGetString("cpmLabel::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,150, 210,20);
	assert( cpmLabel != nullptr );
	cpmLabel->SetToLabel();

	itsSmartTabCB =
		jnew JXTextCheckbox(JGetString("itsSmartTabCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,220, 280,20);
	assert( itsSmartTabCB != nullptr );

	itsPWModRG =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 460,350, 160,80);
	assert( itsPWModRG != nullptr );

	auto* ctrlMetaLabel =
		jnew JXTextRadioButton(JXTEBase::kCtrlMetaPWMod, JGetString("ctrlMetaLabel::EditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 90,20);
	assert( ctrlMetaLabel != nullptr );

	auto* mod2Label =
		jnew JXTextRadioButton(JXTEBase::kMod2PWMod, JGetString("mod2Label::EditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,30, 70,20);
	assert( mod2Label != nullptr );

	auto* mod3Label =
		jnew JXTextRadioButton(JXTEBase::kMod3PWMod, JGetString("mod3Label::EditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,50, 70,20);
	assert( mod3Label != nullptr );

	auto* mod4Label =
		jnew JXTextRadioButton(JXTEBase::kMod4PWMod, JGetString("mod4Label::EditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,30, 60,20);
	assert( mod4Label != nullptr );

	auto* mod5Label =
		jnew JXTextRadioButton(JXTEBase::kMod5PWMod, JGetString("mod5Label::EditTextPrefsDialog::JXLayout"), itsPWModRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,50, 60,20);
	assert( mod5Label != nullptr );

	auto* partialWordTitle =
		jnew JXStaticText(JGetString("partialWordTitle::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,360, 120,60);
	assert( partialWordTitle != nullptr );

	itsCopyWhenSelectCB =
		jnew JXTextCheckbox(JGetString("itsCopyWhenSelectCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,250, 280,20);
	assert( itsCopyWhenSelectCB != nullptr );

	itsColorButton[PrefsManager::kTextColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[PrefsManager::kTextColorIndex-1]::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 400,50, 40,20);
	assert( itsColorButton[PrefsManager::kTextColorIndex-1] != nullptr );

	itsColorButton[PrefsManager::kBackColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[PrefsManager::kBackColorIndex-1]::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 440,50, 80,20);
	assert( itsColorButton[PrefsManager::kBackColorIndex-1] != nullptr );

	auto* changeColorLabel =
		jnew JXStaticText(JGetString("changeColorLabel::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 310,50, 90,20);
	assert( changeColorLabel != nullptr );
	changeColorLabel->SetToLabel();

	itsColorButton[PrefsManager::kSelColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[PrefsManager::kSelColorIndex-1]::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 560,50, 60,20);
	assert( itsColorButton[PrefsManager::kSelColorIndex-1] != nullptr );

	itsColorButton[PrefsManager::kSelLineColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[PrefsManager::kSelLineColorIndex-1]::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 400,70, 50,20);
	assert( itsColorButton[PrefsManager::kSelLineColorIndex-1] != nullptr );

	itsColorButton[PrefsManager::kCaretColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[PrefsManager::kCaretColorIndex-1]::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 520,50, 40,20);
	assert( itsColorButton[PrefsManager::kCaretColorIndex-1] != nullptr );

	itsDefColorsButton =
		jnew JXTextButton(JGetString("itsDefColorsButton::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 400,95, 100,20);
	assert( itsDefColorsButton != nullptr );

	itsInvColorsButton =
		jnew JXTextButton(JGetString("itsInvColorsButton::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 500,95, 100,20);
	assert( itsInvColorsButton != nullptr );

	auto* colorSetsLabel =
		jnew JXStaticText(JGetString("colorSetsLabel::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 310,95, 90,20);
	assert( colorSetsLabel != nullptr );
	colorSetsLabel->SetToLabel();

	itsTabToSpacesCB =
		jnew JXTextCheckbox(JGetString("itsTabToSpacesCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,190, 280,20);
	assert( itsTabToSpacesCB != nullptr );

	itsHomeEndCB =
		jnew JXTextCheckbox(JGetString("itsHomeEndCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 330,470, 300,20);
	assert( itsHomeEndCB != nullptr );

	itsScrollCaretCB =
		jnew JXTextCheckbox(JGetString("itsScrollCaretCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 330,490, 300,20);
	assert( itsScrollCaretCB != nullptr );

	itsSortFnMenuCB =
		jnew JXTextCheckbox(JGetString("itsSortFnMenuCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,390, 280,20);
	assert( itsSortFnMenuCB != nullptr );

	itsPackFnMenuCB =
		jnew JXTextCheckbox(JGetString("itsPackFnMenuCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,430, 280,20);
	assert( itsPackFnMenuCB != nullptr );

	itsColorButton[PrefsManager::kRightMarginColorIndex-1] =
		jnew JXTextButton(JGetString("itsColorButton[PrefsManager::kRightMarginColorIndex-1]::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 450,70, 160,20);
	assert( itsColorButton[PrefsManager::kRightMarginColorIndex-1] != nullptr );

	itsRightMarginCB =
		jnew JXTextCheckbox(JGetString("itsRightMarginCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 330,190, 210,20);
	assert( itsRightMarginCB != nullptr );

	itsRightMarginInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 540,190, 40,20);
	assert( itsRightMarginInput != nullptr );

	itsSampleText =
		jnew SampleText(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,20, 310,25);
	assert( itsSampleText != nullptr );

	itsNSInFnMenuCB =
		jnew JXTextCheckbox(JGetString("itsNSInFnMenuCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,410, 280,20);
	assert( itsNSInFnMenuCB != nullptr );

	auto* wordWrapHint =
		jnew JXStaticText(JGetString("wordWrapHint::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,460, 250,20);
	assert( wordWrapHint != nullptr );
	wordWrapHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	wordWrapHint->SetToLabel();

	itsMiddleButtonPasteCB =
		jnew JXTextCheckbox(JGetString("itsMiddleButtonPasteCB::EditTextPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,270, 280,20);
	assert( itsMiddleButtonPasteCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditTextPrefsDialog"));
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsEmulatorMenu->SetMenuItems(kEmulatorMenuStr);
	itsEmulatorMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsEmulatorMenu->SetToPopupChoice(true, itsEmulatorIndex);
	ListenTo(itsEmulatorMenu);

	itsCreateBackupCB->SetState(doc->WillMakeBackupFile());
	itsOnlyBackupIfNoneCB->SetState(!doc->WillMakeNewBackupEveryOpen());
	itsExtraSpaceWindTitleCB->SetState(doc->WillAllocateTitleSpace());
	itsOpenComplFileOnTopCB->SetState(doc->WillOpenComplFileOnTop());

	TextEditor* te = doc->GetTextEditor();

	itsAutoIndentCB->SetState(te->GetText()->WillAutoIndent());
	itsUseDNDCB->SetState(te->OverrideAllowsDragAndDrop());
	itsLeftToFrontOfTextCB->SetState(te->WillMoveToFrontOfText());

	itsBalanceWhileTypingCB->SetState(te->WillBalanceWhileTyping());
	itsScrollToBalanceCB->SetState(te->WillScrollToBalance());
	itsBeepWhenTypeUnbalancedCB->SetState(te->WillBeepWhenTypeUnbalanced());

	FnMenuUpdater* updater = GetFnMenuUpdater();
	itsSortFnMenuCB->SetState(updater->WillSortFnNames());
	itsNSInFnMenuCB->SetState(updater->WillIncludeNamespace());
	itsPackFnMenuCB->SetState(updater->WillPackFnNames());

	itsSmartTabCB->SetState(te->TabIsSmart());
	itsTabToSpacesCB->SetState(te->GetText()->TabInsertsSpaces());
	itsCopyWhenSelectCB->SetState(te->WillCopyWhenSelect());
	itsMiddleButtonPasteCB->SetState(te->MiddleButtonWillPaste());

	const JFont& font = te->GetText()->GetDefaultFont();
	itsFontMenu->SetFont(font.GetName(), font.GetSize());

	itsTabCharCountInput->SetValue(te->GetTabCharCount());
	itsTabCharCountInput->SetLowerLimit(1);

	itsCRMLineWidthInput->SetValue(te->GetText()->GetCRMLineWidth());
	itsCRMLineWidthInput->SetLowerLimit(1);

	itsUndoDepthInput->SetValue(te->GetText()->GetUndoDepth());
	itsUndoDepthInput->SetLowerLimit(1);

	JSize margin;
	itsRightMarginCB->SetState(te->GetRightMarginWidth(&margin));
	itsRightMarginInput->SetValue(margin);
	itsRightMarginInput->SetLowerLimit(1);

	itsPWModRG->SelectItem(te->GetPartialWordModifier());
	itsHomeEndCB->SetState(JXTEBase::WillUseWindowsHomeEnd());
	itsScrollCaretCB->SetState(TextEditor::CaretWillFollowScroll());

	ListenTo(itsCreateBackupCB);
	ListenTo(itsBalanceWhileTypingCB);
	UpdateDisplay();

	// must do this after SetWindow()

	PrefsManager* prefsMgr = GetPrefsManager();
	for (JUnsignedOffset i=0; i<PrefsManager::kColorCount; i++)
	{
		itsColor[i] = prefsMgr->GetColor(i+1);
		ListenTo(itsColorButton[i]);
	}

	ListenTo(itsDefColorsButton);
	ListenTo(itsInvColorsButton);
	ListenTo(itsFontMenu);
	ListenTo(itsRightMarginCB);

	UpdateSampleText();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
EditTextPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		JXGetHelpManager()->ShowSection("EditorHelp-Prefs");
	}

	else if ((sender == itsCreateBackupCB || sender == itsBalanceWhileTypingCB) &&
			 message.Is(JXCheckbox::kPushed))
	{
		UpdateDisplay();
	}

	else if (sender == itsFontMenu && message.Is(JXChooseMonoFont::kFontChanged))
	{
		UpdateSampleText();
	}
	else if (sender == itsRightMarginCB && message.Is(JXCheckbox::kPushed))
	{
		UpdateSampleText();
	}

	else if (message.Is(JXButton::kPushed) && HandleColorButton(sender))
	{
		// function did all the work
	}

	else if (sender == itsDefColorsButton && message.Is(JXButton::kPushed))
	{
		SetDefaultColors();
	}
	else if (sender == itsInvColorsButton && message.Is(JXButton::kPushed))
	{
		SetReverseVideoColors();
	}

	else if (sender == itsEmulatorMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		itsEmulatorMenu->CheckItem(itsEmulatorIndex);
	}
	else if (sender == itsEmulatorMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* info =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( info != nullptr );
		itsEmulatorIndex = info->GetIndex();
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
EditTextPrefsDialog::UpdateSettings()
{
	TextEditor* te = itsDoc->GetTextEditor();

	JString fontName;
	JSize fontSize;
	itsFontMenu->GetFont(&fontName, &fontSize);
	const bool fontChanged = fontName != te->GetText()->GetDefaultFont().GetName() ||
		fontSize != te->GetText()->GetDefaultFont().GetSize();

	JFontManager* fontMgr = GetDisplay()->GetFontManager();

	JFloat vScrollScale = 1.0;
	if (fontChanged)
	{
		const JFloat h1 = te->GetText()->GetDefaultFont().GetLineHeight(fontMgr);
		const JFloat h2 = JFontManager::GetFont(fontName, fontSize).GetLineHeight(fontMgr);
		vScrollScale    = h2 / h1;
	}

	JInteger tabCharCount;
	bool ok = itsTabCharCountInput->GetValue(&tabCharCount);
	assert( ok );

	JInteger crmLineWidth;
	ok = itsCRMLineWidthInput->GetValue(&crmLineWidth);
	assert( ok );

	JInteger undoDepth;
	ok = itsUndoDepthInput->GetValue(&undoDepth);
	assert( ok );

	JInteger rightMargin;
	ok = itsRightMarginInput->GetValue(&rightMargin);
	assert( ok );

	PrefsManager* prefsMgr = GetPrefsManager();
	const bool textColorChanged =
		itsColor[ PrefsManager::kTextColorIndex-1 ] !=
		prefsMgr->GetColor(PrefsManager::kTextColorIndex);

	// set colors before RecalcStyles() so stylers update themselves

	prefsMgr->SetDefaultFont(fontName, fontSize);
	for (JIndex j=1; j<=PrefsManager::kColorCount; j++)
	{
		prefsMgr->SetColor(j, itsColor[j-1]);
	}

	JPtrArray<TextDocument>* docList = GetDocumentManager()->GetTextDocList();
	const JSize docCount = docList->GetItemCount();

	JProgressDisplay* pg = JNewPG();
	pg->FixedLengthProcessBeginning(docCount, JGetString("UpdatingPrefs::EditTextPrefsDialog"), false, true);

	for (JIndex i=1; i<=docCount; i++)
	{
		TextDocument* doc = docList->GetItem(i);

		doc->ShouldMakeBackupFile(itsCreateBackupCB->IsChecked());
		doc->ShouldMakeNewBackupEveryOpen(!itsOnlyBackupIfNoneCB->IsChecked());
		doc->ShouldAllocateTitleSpace(itsExtraSpaceWindTitleCB->IsChecked());
		doc->ShouldOpenComplFileOnTop(itsOpenComplFileOnTopCB->IsChecked());

		te = doc->GetTextEditor();

		if (itsEmulatorIndex != itsOrigEmulatorIndex)
		{
			JTEKeyHandler* handler;
			InstallEmulator(kMenuIndexToEmulator[ itsEmulatorIndex-1 ], te, &handler);
		}

		te->GetText()->ShouldAutoIndent(itsAutoIndentCB->IsChecked());
		te->OverrideShouldAllowDragAndDrop(itsUseDNDCB->IsChecked());
		te->ShouldMoveToFrontOfText(itsLeftToFrontOfTextCB->IsChecked());

		te->ShouldBalanceWhileTyping(itsBalanceWhileTypingCB->IsChecked());
		te->ShouldScrollToBalance(itsScrollToBalanceCB->IsChecked());
		te->ShouldBeepWhenTypeUnbalanced(itsBeepWhenTypeUnbalancedCB->IsChecked());

		te->TabShouldBeSmart(itsSmartTabCB->IsChecked());
		te->GetText()->TabShouldInsertSpaces(itsTabToSpacesCB->IsChecked());

		if (fontChanged)
		{
			JXScrollbar *hScrollbar, *vScrollbar;
			const bool ok = te->GetScrollbars(&hScrollbar, &vScrollbar);
			assert( ok );
			vScrollbar->PrepareForScaledMaxValue(vScrollScale);

			te->SetFont(fontName, fontSize, tabCharCount);
		}
		else
		{
			te->SetTabCharCount(tabCharCount);
		}

		te->GetText()->SetCRMLineWidth(crmLineWidth);
		te->GetText()->SetUndoDepth(undoDepth);
		te->SetRightMarginWidth(itsRightMarginCB->IsChecked(), rightMargin);

		te->GetText()->SetDefaultFontStyle(itsColor [ PrefsManager::kTextColorIndex-1 ]);
		te->SetBackColor(itsColor [ PrefsManager::kBackColorIndex-1 ]);
		te->SetFocusColor(itsColor [ PrefsManager::kBackColorIndex-1 ]);
		te->SetCaretColor(itsColor [ PrefsManager::kCaretColorIndex-1 ]);
		te->SetSelectionColor(itsColor [ PrefsManager::kSelColorIndex-1 ]);
		te->SetSelectionOutlineColor(itsColor [ PrefsManager::kSelLineColorIndex-1 ]);
		te->SetRightMarginColor(itsColor [ PrefsManager::kRightMarginColorIndex-1 ]);

		if (textColorChanged)
		{
			te->RecalcStyles();
		}

		// force update of insertion font

		JIndex caretIndex;
		if (te->GetCaretLocation(&caretIndex))
		{
			te->SetCaretLocation(caretIndex);
		}

		pg->IncrementProgress();
	}

	FnMenuUpdater* updater = GetFnMenuUpdater();
	updater->ShouldSortFnNames(itsSortFnMenuCB->IsChecked());
	updater->ShouldIncludeNamespace(itsNSInFnMenuCB->IsChecked());
	updater->ShouldPackFnNames(itsPackFnMenuCB->IsChecked());

	JXTEBase::SetPartialWordModifier(
		(JXTEBase::PartialWordModifier) itsPWModRG->GetSelectedItem());

	JXTEBase::ShouldUseWindowsHomeEnd(itsHomeEndCB->IsChecked());
	TextEditor::CaretShouldFollowScroll(itsScrollCaretCB->IsChecked());
	JTextEditor::ShouldCopyWhenSelect(itsCopyWhenSelectCB->IsChecked());
	JXTEBase::MiddleButtonShouldPaste(itsMiddleButtonPasteCB->IsChecked());

	GetSearchTextDialog()->SetFont(JFontManager::GetFont(fontName, fontSize));

	itsDoc->JPrefObject::WritePrefs();

	if (itsEmulatorIndex != itsOrigEmulatorIndex)
	{
		prefsMgr->SetEmulator(kMenuIndexToEmulator[ itsEmulatorIndex-1 ]);
	}

	WriteSharedPrefs(true);

	pg->ProcessFinished();
	jdelete pg;
}

/******************************************************************************
 HandleColorButton (private)

	Returns true if the sender is one of our color buttons.

 ******************************************************************************/

bool
EditTextPrefsDialog::HandleColorButton
	(
	JBroadcaster* sender
	)
{
	JIndex colorIndex = 0;
	for (JUnsignedOffset i=0; i<PrefsManager::kColorCount; i++)
	{
		if (sender == itsColorButton[i])
		{
			colorIndex = i+1;
			break;
		}
	}

	if (colorIndex == 0)
	{
		return false;
	}

	auto* dlog = jnew JXChooseColorDialog(itsColor[colorIndex-1]);

	if (dlog->DoDialog())
	{
		ChangeColor(colorIndex, dlog->GetColor());
	}

	return true;
}

/******************************************************************************
 SetDefaultColors (private)

 ******************************************************************************/

void
EditTextPrefsDialog::SetDefaultColors()
{
	ChangeColor(PrefsManager::kTextColorIndex,        JColorManager::GetBlackColor());
	ChangeColor(PrefsManager::kBackColorIndex,        JColorManager::GetWhiteColor());
	ChangeColor(PrefsManager::kCaretColorIndex,       JColorManager::GetRedColor());
	ChangeColor(PrefsManager::kSelColorIndex,         JColorManager::GetDefaultSelectionColor());
	ChangeColor(PrefsManager::kSelLineColorIndex,     JColorManager::GetBlueColor());
	ChangeColor(PrefsManager::kRightMarginColorIndex, JColorManager::GetGrayColor(70));
}

/******************************************************************************
 SetReverseVideoColors (private)

 ******************************************************************************/

void
EditTextPrefsDialog::SetReverseVideoColors()
{
	ChangeColor(PrefsManager::kTextColorIndex,        JColorManager::GetWhiteColor());
	ChangeColor(PrefsManager::kBackColorIndex,        JColorManager::GetBlackColor());
	ChangeColor(PrefsManager::kCaretColorIndex,       JColorManager::GetWhiteColor());
	ChangeColor(PrefsManager::kSelColorIndex,         JColorManager::GetBlueColor());
	ChangeColor(PrefsManager::kSelLineColorIndex,     JColorManager::GetCyanColor());
	ChangeColor(PrefsManager::kRightMarginColorIndex, JColorManager::GetGrayColor(80));
}

/******************************************************************************
 ChangeColor (private)

 ******************************************************************************/

void
EditTextPrefsDialog::ChangeColor
	(
	const JIndex	colorIndex,
	const JColorID	color
	)
{
	assert( PrefsManager::ColorIndexValid(colorIndex) );

	itsColor [ colorIndex-1 ] = color;
	UpdateSampleText();
}

/******************************************************************************
 UpdateSampleText (private)

 ******************************************************************************/

void
EditTextPrefsDialog::UpdateSampleText()
{
	JString name;
	JSize size;
	itsFontMenu->GetFont(&name, &size);
	itsSampleText->SetFont(
		JFontManager::GetFont(name, size,
			JFontStyle(itsColor [ PrefsManager::kTextColorIndex-1 ])));

	itsSampleText->SetBackColor(itsColor [ PrefsManager::kBackColorIndex-1 ] );
	itsSampleText->SetFocusColor(itsColor [ PrefsManager::kBackColorIndex-1 ] );

	itsSampleText->SetCaretColor(itsColor [ PrefsManager::kCaretColorIndex-1 ] );
	itsSampleText->SetSelectionColor(itsColor [ PrefsManager::kSelColorIndex-1 ] );
	itsSampleText->SetSelectionOutlineColor(itsColor [ PrefsManager::kSelLineColorIndex-1 ] );

	itsSampleText->ShowRightMargin(itsRightMarginCB->IsChecked(),
								   itsColor [ PrefsManager::kRightMarginColorIndex-1 ] );
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
EditTextPrefsDialog::UpdateDisplay()
{
	if (itsCreateBackupCB->IsChecked())
	{
		itsOnlyBackupIfNoneCB->Activate();
	}
	else
	{
		itsOnlyBackupIfNoneCB->Deactivate();
	}

	if (itsBalanceWhileTypingCB->IsChecked())
	{
		itsScrollToBalanceCB->Activate();
		itsBeepWhenTypeUnbalancedCB->Activate();
	}
	else
	{
		itsScrollToBalanceCB->Deactivate();
		itsBeepWhenTypeUnbalancedCB->Deactivate();
	}
}
