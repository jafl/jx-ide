/******************************************************************************
 EditProjPrefsDialog.h

	Interface for the EditProjPrefsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditProjPrefsDialog
#define _H_EditProjPrefsDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include "ProjectTable.h"		// for DropFileAction

class JXTextButton;
class JXTextCheckbox;
class JXRadioGroup;

class EditProjPrefsDialog : public JXModalDialogDirector
{
public:

	EditProjPrefsDialog(const bool reopenTextFiles,
						const bool doubleSpaceCompile,
						const bool rebuildMakefileDaily,
						const ProjectTable::DropFileAction dropFileAction);

	~EditProjPrefsDialog() override;

	void	UpdateSettings();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsReopenTextFilesCB;
	JXTextCheckbox* itsDoubleSpaceCB;
	JXTextCheckbox* itsRebuildMakefileDailyCB;
	JXRadioGroup*   itsDropFileActionRG;
	JXTextButton*   itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const bool reopenTextFiles,
						const bool doubleSpaceCompile,
						const bool rebuildMakefileDaily,
						const ProjectTable::DropFileAction dropFileAction);
};

#endif
