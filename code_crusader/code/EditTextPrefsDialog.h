/******************************************************************************
 EditTextPrefsDialog.h

	Interface for the EditTextPrefsDialog class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditTextPrefsDialog
#define _H_EditTextPrefsDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include "PrefsManager.h"		// for kColorCount

class TextDocument;
class SampleText;

class JXTextButton;
class JXTextCheckbox;
class JXTextMenu;
class JXInputField;
class JXIntegerInput;
class JXCharInput;
class JXRadioGroup;
class JXChooseMonoFont;

class EditTextPrefsDialog : public JXModalDialogDirector
{
public:

	EditTextPrefsDialog(TextDocument* doc);

	~EditTextPrefsDialog() override;

	void	UpdateSettings();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	TextDocument*	itsDoc;		// not owned
	JColorID		itsColor [ PrefsManager::kColorCount ];
	JXTextButton*	itsColorButton [ PrefsManager::kColorCount ];
	JIndex			itsEmulatorIndex;
	JIndex			itsOrigEmulatorIndex;

// begin JXLayout

	JXChooseMonoFont* itsFontMenu;
	SampleText*       itsSampleText;
	JXTextMenu*       itsEmulatorMenu;
	JXTextButton*     itsDefColorsButton;
	JXTextButton*     itsInvColorsButton;
	JXTextCheckbox*   itsUseDNDCB;
	JXTextCheckbox*   itsAutoIndentCB;
	JXTextCheckbox*   itsTabToSpacesCB;
	JXTextCheckbox*   itsRightMarginCB;
	JXTextCheckbox*   itsSmartTabCB;
	JXTextCheckbox*   itsCreateBackupCB;
	JXTextCheckbox*   itsCopyWhenSelectCB;
	JXTextCheckbox*   itsOnlyBackupIfNoneCB;
	JXTextCheckbox*   itsMiddleButtonPasteCB;
	JXTextCheckbox*   itsOpenComplFileOnTopCB;
	JXTextCheckbox*   itsBalanceWhileTypingCB;
	JXTextCheckbox*   itsExtraSpaceWindTitleCB;
	JXTextCheckbox*   itsScrollToBalanceCB;
	JXTextCheckbox*   itsBeepWhenTypeUnbalancedCB;
	JXRadioGroup*     itsPWModRG;
	JXTextCheckbox*   itsSortFnMenuCB;
	JXTextCheckbox*   itsNSInFnMenuCB;
	JXTextCheckbox*   itsPackFnMenuCB;
	JXTextCheckbox*   itsLeftToFrontOfTextCB;
	JXTextCheckbox*   itsHomeEndCB;
	JXTextButton*     itsHelpButton;
	JXTextCheckbox*   itsScrollCaretCB;
	JXIntegerInput*   itsTabCharCountInput;
	JXIntegerInput*   itsCRMLineWidthInput;
	JXIntegerInput*   itsUndoDepthInput;
	JXIntegerInput*   itsRightMarginInput;

// end JXLayout

private:

	void	BuildWindow(TextDocument* doc);

	bool	HandleColorButton(JBroadcaster* sender);
	void	SetDefaultColors();
	void	SetReverseVideoColors();
	void	ChangeColor(const JIndex colorIndex, const JColorID color);
	void	UpdateSampleText();

	void	UpdateDisplay();
};

#endif
