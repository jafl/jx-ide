/******************************************************************************
 RunCommandDialog.h

	Copyright © 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_RunCommandDialog
#define _H_RunCommandDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXInputField;
class JXTextButton;
class JXTextCheckbox;
class JXTextMenu;
class JXPathHistoryMenu;
class ProjectDocument;
class TextDocument;
class CommandManager;
class CommandPathInput;

class RunCommandDialog : public JXModalDialogDirector, public JPrefObject
{
public:

	RunCommandDialog(ProjectDocument* projDoc, TextDocument* textDoc);
	RunCommandDialog(ProjectDocument* projDoc,
					 const JPtrArray<JString>& fullNameList,
					 const JArray<JIndex>& lineIndexList);

	~RunCommandDialog() override;

	void	Exec();

	void	Activate() override;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const  override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ProjectDocument*	itsProjDoc;			// can be nullptr
	TextDocument*		itsTextDoc;			// can be nullptr
	JPtrArray<JString>*	itsFullNameList;	// can be nullptr
	JArray<JIndex>*		itsLineIndexList;	// can be nullptr

// begin JXLayout

	CommandPathInput*  itsPathInput;
	JXPathHistoryMenu* itsPathHistoryMenu;
	JXTextButton*      itsChoosePathButton;
	JXTextButton*      itsChooseCmdButton;
	JXTextCheckbox*    itsIsMakeCB;
	JXTextCheckbox*    itsUseWindowCB;
	JXTextCheckbox*    itsIsCVSCB;
	JXTextCheckbox*    itsRaiseCB;
	JXTextCheckbox*    itsBeepCB;
	JXTextCheckbox*    itsSaveAllCB;
	JXTextCheckbox*    itsOneAtATimeCB;
	JXTextButton*      itsRunButton;
	JXTextMenu*        itsSaveCmdMenu;
	JXTextButton*      itsHelpButton;
	JXInputField*      itsCmdInput;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();

	void	UpdateSaveCmdMenu();
	void	HandleSaveCmdMenu(const JIndex item);
	void	AddCommandToMenu(CommandManager* mgr);
};

#endif
