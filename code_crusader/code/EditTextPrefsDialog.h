/******************************************************************************
 EditTextPrefsDialog.h

	Interface for the EditTextPrefsDialog class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditTextPrefsDialog
#define _H_EditTextPrefsDialog

#include <jx-af/jx/JXDialogDirector.h>
#include "PrefsManager.h"		// need definition of kColorCount

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
class JXChooseColorDialog;

class EditTextPrefsDialog : public JXDialogDirector
{
public:

	EditTextPrefsDialog(TextDocument* doc);

	virtual ~EditTextPrefsDialog();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	TextDocument*	itsDoc;		// not owned
	JColorID		itsColor [ PrefsManager::kColorCount ];
	JXTextButton*	itsColorButton [ PrefsManager::kColorCount ];
	JIndex			itsEmulatorIndex;
	JIndex			itsOrigEmulatorIndex;

	// used when selecting custom color

	JXChooseColorDialog*	itsChooseColorDialog;	// can be nullptr
	JIndex					itsChooseColorIndex;

// begin JXLayout

	JXIntegerInput*   itsTabCharCountInput;
	JXIntegerInput*   itsCRMLineWidthInput;
	JXIntegerInput*   itsUndoDepthInput;
	JXTextCheckbox*   itsCreateBackupCB;
	JXTextCheckbox*   itsAutoIndentCB;
	JXTextCheckbox*   itsExtraSpaceWindTitleCB;
	JXTextCheckbox*   itsOpenComplFileOnTopCB;
	JXChooseMonoFont* itsFontMenu;
	JXTextMenu*       itsEmulatorMenu;
	JXTextCheckbox*   itsUseDNDCB;
	JXTextCheckbox*   itsOnlyBackupIfNoneCB;
	JXTextCheckbox*   itsLeftToFrontOfTextCB;
	JXTextButton*     itsHelpButton;
	JXTextCheckbox*   itsBalanceWhileTypingCB;
	JXTextCheckbox*   itsScrollToBalanceCB;
	JXTextCheckbox*   itsBeepWhenTypeUnbalancedCB;
	JXTextCheckbox*   itsSmartTabCB;
	JXRadioGroup*     itsPWModRG;
	JXTextCheckbox*   itsCopyWhenSelectCB;
	JXTextButton*     itsDefColorsButton;
	JXTextButton*     itsInvColorsButton;
	JXTextCheckbox*   itsTabToSpacesCB;
	JXTextCheckbox*   itsHomeEndCB;
	JXTextCheckbox*   itsScrollCaretCB;
	JXTextCheckbox*   itsSortFnMenuCB;
	JXTextCheckbox*   itsPackFnMenuCB;
	JXTextCheckbox*   itsRightMarginCB;
	JXIntegerInput*   itsRightMarginInput;
	SampleText*       itsSampleText;
	JXTextCheckbox*   itsNSInFnMenuCB;
	JXTextCheckbox*   itsMiddleButtonPasteCB;

// end JXLayout

private:

	void	BuildWindow(TextDocument* doc);
	void	UpdateSettings();

	bool	HandleColorButton(JBroadcaster* sender);
	void		SetDefaultColors();
	void		SetReverseVideoColors();
	void		ChangeColor(const JIndex colorIndex, const JColorID color);
	void		UpdateSampleText();

	void	UpdateDisplay();

	// not allowed

	EditTextPrefsDialog(const EditTextPrefsDialog& source);
	const EditTextPrefsDialog& operator=(const EditTextPrefsDialog& source);
};

#endif
