/******************************************************************************
 RunTEScriptDialog.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_RunTEScriptDialog
#define _H_RunTEScriptDialog

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JTextEditor;
class JXTextButton;
class JXTextCheckbox;
class JXInputField;
class JXStringHistoryMenu;

class RunTEScriptDialog : public JXWindowDirector, public JPrefObject
{
public:

	RunTEScriptDialog(JXDirector* supervisor);

	~RunTEScriptDialog() override;

	void	Activate() override;

	bool	RunScript();
	bool	RunSimpleScript(const JString& scriptName, JTextEditor* te,
							const JString& fileName);

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXStringHistoryMenu* itsHistoryMenu;
	JXTextCheckbox*      itsStayOpenCB;
	JXTextButton*        itsCloseButton;
	JXTextButton*        itsHelpButton;
	JXTextButton*        itsRunButton;
	JXInputField*        itsCmdInput;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();
	bool	RunScript(JTextEditor* te, const JString& fullName);
	bool	RunScript(const JString& cmd, JTextEditor* te,
					  const JString& fullName);
	void	ReplaceVariables(JString* cmd, JTextEditor* te,
							 const JString& fullName);
};

#endif
