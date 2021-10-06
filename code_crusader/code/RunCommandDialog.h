/******************************************************************************
 RunCommandDialog.h

	Copyright © 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_RunCommandDialog
#define _H_RunCommandDialog

#include <jx-af/jx/JXDialogDirector.h>
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

class RunCommandDialog : public JXDialogDirector, public JPrefObject
{
public:

	RunCommandDialog(ProjectDocument* projDoc, TextDocument* textDoc);
	RunCommandDialog(ProjectDocument* projDoc,
					   const JPtrArray<JString>& fullNameList,
					   const JArray<JIndex>& lineIndexList);

	virtual ~RunCommandDialog();

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

	JXTextButton*      itsRunButton;
	JXTextButton*      itsHelpButton;
	CommandPathInput*  itsPathInput;
	JXTextCheckbox*    itsSaveAllCB;
	JXTextCheckbox*    itsOneAtATimeCB;
	JXTextCheckbox*    itsUseWindowCB;
	JXTextCheckbox*    itsIsMakeCB;
	JXInputField*      itsCmdInput;
	JXPathHistoryMenu* itsPathHistoryMenu;
	JXTextButton*      itsChoosePathButton;
	JXTextButton*      itsChooseCmdButton;
	JXTextCheckbox*    itsRaiseCB;
	JXTextCheckbox*    itsBeepCB;
	JXTextMenu*        itsSaveCmdMenu;
	JXTextCheckbox*    itsIsCVSCB;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();
	void	Exec();

	void	UpdateSaveCmdMenu();
	void	HandleSaveCmdMenu(const JIndex item);
	void	AddCommandToMenu(CommandManager* mgr);
};

#endif
