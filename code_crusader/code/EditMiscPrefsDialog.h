/******************************************************************************
 EditMiscPrefsDialog.h

	Interface for the EditMiscPrefsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditMiscPrefsDialog
#define _H_EditMiscPrefsDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXRadioGroup;
class JXTextCheckbox;

class EditMiscPrefsDialog : public JXDialogDirector
{
public:

	EditMiscPrefsDialog();

	~EditMiscPrefsDialog();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsSaveAllCB;
	JXTextCheckbox* itsCloseAllCB;
	JXTextCheckbox* itsQuitCB;
	JXTextCheckbox* itsNewEditorCB;
	JXTextCheckbox* itsNewProjectCB;
	JXTextCheckbox* itsChooseFileCB;
	JXTextCheckbox* itsReopenLastCB;
	JXTextCheckbox* itsCopyWhenSelectCB;
	JXTextCheckbox* itsCloseUnsavedCB;
	JXTextCheckbox* itsMacStyleCB;
	JXTextCheckbox* itsOpenOldProjCB;
	JXTextCheckbox* itsFocusInDockCB;
	JXTextCheckbox* itsMiddleButtonPasteCB;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateSettings();
};

#endif
