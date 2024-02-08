/******************************************************************************
 EditMiscPrefsDialog.h

	Interface for the EditMiscPrefsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditMiscPrefsDialog
#define _H_EditMiscPrefsDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXRadioGroup;
class JXTextCheckbox;

class EditMiscPrefsDialog : public JXModalDialogDirector
{
public:

	EditMiscPrefsDialog();

	~EditMiscPrefsDialog() override;

	void	UpdateSettings();

private:

// begin JXLayout

	JXTextCheckbox* itsMacStyleCB;
	JXTextCheckbox* itsCopyWhenSelectCB;
	JXTextCheckbox* itsMiddleButtonPasteCB;
	JXTextCheckbox* itsFocusInDockCB;
	JXTextCheckbox* itsNewEditorCB;
	JXTextCheckbox* itsOpenOldProjCB;
	JXTextCheckbox* itsNewProjectCB;
	JXTextCheckbox* itsCloseUnsavedCB;
	JXTextCheckbox* itsReopenLastCB;
	JXTextCheckbox* itsSaveAllCB;
	JXTextCheckbox* itsCloseAllCB;
	JXTextCheckbox* itsChooseFileCB;
	JXTextCheckbox* itsQuitCB;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
