/******************************************************************************
 ExtEditorDialog.h

	Interface for the ExtEditorDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_ExtEditorDialog
#define _H_ExtEditorDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JString;
class JXTextCheckbox;
class JXInputField;

class ExtEditorDialog : public JXModalDialogDirector
{
public:

	ExtEditorDialog(const bool editLocally,
					const JString& editFileCmd,
					const JString& editFileLineCmd,
					const bool editBinaryLocally,
					const JString& editBinaryFileCmd);

	~ExtEditorDialog() override;

	void	GetPrefs(bool* editTextLocally,
					 JString* editTextFileCmd,
					 JString* editTextFileLineCmd,
					 bool* editBinaryLocally,
					 JString* editBinaryFileCmd) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXInputField*   itsEditTextFileCmdInput;
	JXInputField*   itsEditTextFileLineCmdInput;
	JXTextCheckbox* itsExtTextEditorCB;
	JXInputField*   itsEditBinaryFileCmdInput;
	JXTextCheckbox* itsExtBinaryEditorCB;

// end JXLayout

private:

	void	BuildWindow(const bool editTextLocally,
						const JString& editTextFileCmd,
						const JString& editTextFileLineCmd,
						const bool editBinaryLocally,
						const JString& editBinaryFileCmd);

	void	UpdateDisplay();
};

#endif
