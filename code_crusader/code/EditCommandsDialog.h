/******************************************************************************
 EditCommandsDialog.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditCommandsDialog
#define _H_EditCommandsDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include "CommandManager.h"	// for CmdInfo
#include <jx-af/jcore/JString.h>

class JXTextButton;
class JXVertPartition;
class JXInputField;
class ProjectDocument;
class CommandTable;

class EditCommandsDialog : public JXModalDialogDirector, public JPrefObject
{
public:

	EditCommandsDialog(ProjectDocument* projDoc);

	~EditCommandsDialog() override;

	void	UpdateCommands();

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CommandManager*	itsCmdMgr;			// can be nullptr
	JString			itsThisTableSetup;	// used when itsThisTable == nullptr

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout

// begin allProjectsLayout

	CommandTable* itsAllTable;

// end allProjectsLayout

// begin thisProjectLayout

	CommandTable* itsThisTable;
	JXInputField* itsMakeDependCmd;

// end thisProjectLayout

private:

	void	BuildWindow(ProjectDocument* projDoc);
};

#endif
