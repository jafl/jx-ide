/******************************************************************************
 EditProjPrefsDialog.h

	Interface for the EditProjPrefsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditProjPrefsDialog
#define _H_EditProjPrefsDialog

#include <jx-af/jx/JXDialogDirector.h>
#include "ProjectTable.h"		// need definition of DropFileAction

class JXTextButton;
class JXTextCheckbox;
class JXRadioGroup;

class EditProjPrefsDialog : public JXDialogDirector
{
public:

	EditProjPrefsDialog(const bool reopenTextFiles,
						  const bool doubleSpaceCompile,
						  const bool rebuildMakefileDaily,
						  const ProjectTable::DropFileAction dropFileAction);

	virtual ~EditProjPrefsDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsReopenTextFilesCB;
	JXTextButton*   itsHelpButton;
	JXRadioGroup*   itsDropFileActionRG;
	JXTextCheckbox* itsDoubleSpaceCB;
	JXTextCheckbox* itsRebuildMakefileDailyCB;

// end JXLayout

private:

	void	BuildWindow(const bool reopenTextFiles,
						const bool doubleSpaceCompile,
						const bool rebuildMakefileDaily,
						const ProjectTable::DropFileAction dropFileAction);
	void	UpdateSettings();

	// not allowed

	EditProjPrefsDialog(const EditProjPrefsDialog& source);
	const EditProjPrefsDialog& operator=(const EditProjPrefsDialog& source);
};

#endif
