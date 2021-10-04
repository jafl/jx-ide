/******************************************************************************
 EditCommandsDialog.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditCommandsDialog
#define _H_EditCommandsDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include "CommandManager.h"	// need defn of CmdInfo
#include <jx-af/jcore/JString.h>

class JXTextButton;
class JXVertPartition;
class JXInputField;
class ProjectDocument;
class CommandTable;

class EditCommandsDialog : public JXDialogDirector, public JPrefObject
{
public:

	EditCommandsDialog(ProjectDocument* projDoc);

	virtual ~EditCommandsDialog();

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CommandTable*		itsAllTable;
	CommandManager*	itsCmdMgr;			// can be nullptr
	CommandTable*		itsThisTable;		// nullptr if itsCmdMgr == nullptr
	JString				itsThisTableSetup;	// used when itsThisTable == nullptr

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout

// begin allProjectsLayout


// end allProjectsLayout

// begin thisProjectLayout

	JXInputField* itsMakeDependCmd;

// end thisProjectLayout

private:

	void	BuildWindow(ProjectDocument* projDoc);

	// not allowed

	EditCommandsDialog(const EditCommandsDialog& source);
	const EditCommandsDialog& operator=(const EditCommandsDialog& source);
};

#endif