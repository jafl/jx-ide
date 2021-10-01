/******************************************************************************
 EditSymbolPrefsDialog.h

	Interface for the EditSymbolPrefsDialog class

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditSymbolPrefsDialog
#define _H_EditSymbolPrefsDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextButton;
class JXTextCheckbox;

class EditSymbolPrefsDialog : public JXDialogDirector
{
public:

	EditSymbolPrefsDialog(const bool raiseTreeOnRightClick);

	virtual ~EditSymbolPrefsDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsRaiseTreeOnRightClickCB;
	JXTextButton*   itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const bool raiseTreeOnRightClick);
	void	UpdateSettings();

	// not allowed

	EditSymbolPrefsDialog(const EditSymbolPrefsDialog& source);
	const EditSymbolPrefsDialog& operator=(const EditSymbolPrefsDialog& source);
};

#endif
